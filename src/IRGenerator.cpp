#include "IRGenerator.h"

IRGenerator::IRGenerator() {
    signatures = std::make_shared<syntax_tree::ASTNode>("SIGNATURES");
}

void IRGenerator::generate(std::shared_ptr<syntax_tree::ASTNode> node) {
    std::vector<std::shared_ptr<syntax_tree::ASTNode>> decls;

    if (node->getNodeType() == "DEFINITIONS") {
        decls = node->getStatements();
    }
    else if (node->getNodeType() == "LET") {
        decls = node->getStatement(0)->getStatements();
    }
    else if (node->getNodeType() == "WHERE") {
        decls = node->getStatement(1)->getStatements();
    }

    auto grouped = GroupingService::getInstance().groupByName(decls);
    std::vector<std::shared_ptr<syntax_tree::ASTNode>> newDecls;
    for (const auto& name : grouped.order) {
        auto& decls = grouped.groups[name];

        auto signature = extractSignature(decls);
        auto fn = buildFunction(name, decls);

        newDecls.push_back(fn);
        if (signature != nullptr) signatures->addStatement(signature);
    }

    if (node->getNodeType() == "DEFINITIONS") {
        node->setStatements(newDecls);
    }
    else if (node->getNodeType() == "LET") {
        node->getStatement(0)->setStatements(newDecls);
    }
    else if (node->getNodeType() == "WHERE") {
        node->getStatement(1)->setStatements(newDecls);
    }
}

std::shared_ptr<syntax_tree::ASTNode>& IRGenerator::getSignatures() {
    return signatures;
}

std::shared_ptr<syntax_tree::ASTNode> IRGenerator::buildFunction(const std::string &name, 
                                                                    const std::vector<std::shared_ptr<syntax_tree::ASTNode>> &decls)
{
    size_t arity = getArity(decls);
    if (arity == 0) {
        auto fn = std::make_shared<syntax_tree::Definition>("DEF");
        fn->addStatement(std::make_shared<syntax_tree::Identifier>("Identifier", name));
        fn->addStatement(getBody(decls));
        return fn;
    }

    std::shared_ptr<syntax_tree::ASTNode> lambda, body;
    std::vector<std::shared_ptr<syntax_tree::ASTNode>> params;
    if (decls.size() == 1 || decls.size() == 2 && (decls[0]->getNodeType() == "SIGNATURE" || decls[1]->getNodeType() == "SIGNATURE")) {
        params = getParams(decls);
        body = getBody(decls);
    }
    else {
        params = generateParams(arity);
        body = buildCase(params, decls);
    }

    lambda = buildLambda(params, body);

    auto fn = std::make_shared<syntax_tree::Definition>("DEF");

    fn->addStatement(
        std::make_shared<syntax_tree::Identifier>("Identifier", name)
    );

    fn->addStatement(lambda);

    return fn;
}

size_t IRGenerator::getArity(std::vector<std::shared_ptr<syntax_tree::ASTNode>> decls) {
    for (auto& n : decls) {
        if (n->getNodeType() == "DEF" || n->getNodeType() == "=") {
            return n->getStatement(0)->getStatementCount();
        }
    }
    return 0;
}

std::vector<std::shared_ptr<syntax_tree::ASTNode>> IRGenerator::getParams(std::vector<std::shared_ptr<syntax_tree::ASTNode>> decls)
{
    for (size_t i = 0; i < decls.size(); i++)
    {
        if (decls[i]->getNodeType() == "DEF" || decls[i]->getNodeType() == "=") {
            return decls[i]->getStatement(0)->getStatements();
        }
    }
    return std::vector<std::shared_ptr<syntax_tree::ASTNode>>();
}

std::shared_ptr<syntax_tree::ASTNode> IRGenerator::getBody(std::vector<std::shared_ptr<syntax_tree::ASTNode>> decls)
{
    for (size_t i = 0; i < decls.size(); i++)
    {
        if (decls[i]->getNodeType() == "DEF" || decls[i]->getNodeType() == "=") {
            return decls[i]->getStatement(1);
        }
    }
    return std::shared_ptr<syntax_tree::ASTNode>();
}

std::vector<std::shared_ptr<syntax_tree::ASTNode>> IRGenerator::generateParams(size_t arity)
{
    std::vector<std::shared_ptr<syntax_tree::ASTNode>> params;

    for (size_t i = 0; i < arity; ++i)
    {
        auto name = "v" + std::to_string(i+1);

        params.push_back(
            std::make_shared<syntax_tree::Identifier>(
                "Identifier",
                name
            )
        );
    }

    return params;
}

std::shared_ptr<syntax_tree::ASTNode> IRGenerator::extractSignature(std::vector<std::shared_ptr<syntax_tree::ASTNode>> decl) {
    int i = 0;
    for (auto& d : decl) {
        if (d->getNodeType() == "SIGNATURE") {
            return d;
        }
        i++;
    }
    return nullptr;
}

std::vector<Clause> IRGenerator::buildClauses(const std::vector<std::shared_ptr<syntax_tree::ASTNode>> &decls) {
    std::vector<Clause> clauses;

    for (auto& d : decls)
    {
        if (d->getNodeType() != "SIGNATURE") {
            Clause c;

            if (d->getStatement(0)->getStatementCount() > 0) {
                c.patterns = d->getStatement(0)->getStatements();
            }
        
            c.body = d->getStatement(1);

            clauses.push_back(c);
        }
    }

    return clauses;
}

std::shared_ptr<syntax_tree::ASTNode> IRGenerator::buildCase(std::vector<std::shared_ptr<syntax_tree::ASTNode>> params,
                                                                const std::vector<std::shared_ptr<syntax_tree::ASTNode>>& decls)
{
    auto clauses = buildClauses(decls);

    return compileMatch(params, clauses);
}

std::shared_ptr<syntax_tree::ASTNode> IRGenerator::buildLambda(const std::vector<std::shared_ptr<syntax_tree::ASTNode>> &params, 
                                                                std::shared_ptr<syntax_tree::ASTNode> body)
{
    using namespace syntax_tree;

    std::shared_ptr<ASTNode> result = body;

    for (auto it = params.rbegin(); it != params.rend(); ++it)
    {
        auto lambda = std::make_shared<Lambda>("λ");

        lambda->addStatement(*it);   // параметр
        lambda->addStatement(result); // тело

        result = lambda;
    }

    return result;
}

std::shared_ptr<syntax_tree::ASTNode> IRGenerator::compileMatch(std::vector<std::shared_ptr<syntax_tree::ASTNode>> vars, 
                                                                std::vector<Clause> clauses)
{
    using namespace syntax_tree;

    if (vars.empty()) {
        return clauses[0].body;
    }

    auto var = vars[0];

    auto caseNode = std::make_shared<Fatbar>("CASE");
    if (vars.size() == 1) {
        caseNode->addStatement(vars[0]);
    }
    else {
        auto tuple = std::make_shared<Tuple>("TUPLE");
        tuple->setStatements(vars);

        caseNode->addStatement(tuple);
    }

    auto alts = std::make_shared<ASTNode>("ALTS");
    for (auto& c : clauses)
    {
        auto alt = std::make_shared<ASTNode>("ALT");

        if (c.patterns.size()==1) {
            alt->addStatement(c.patterns[0]);
        }
        else {
            auto patterns_tuple = std::make_shared<Tuple>("TUPLE_PATTERN");
            patterns_tuple->setStatements(c.patterns);
            alt->addStatement(patterns_tuple);
        }
        
        alt->addStatement(c.body);

        alts->addStatement(alt);
    }

    caseNode->addStatement(alts);

    return caseNode;
}