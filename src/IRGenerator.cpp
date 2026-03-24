#include "IRGenerator.h"

void IRGenerator::generate(std::shared_ptr<syntax_tree::ASTNode> node, std::unordered_map<std::string, std::vector<std::shared_ptr<syntax_tree::ASTNode>>> groups)
{
    std::vector<std::shared_ptr<syntax_tree::ASTNode>> newDecls;

    for (auto& [name, decls] : groups)
    {
        auto signature = extractSignature(decls);
        auto fn = buildFunction(name, decls);
        newDecls.insert(newDecls.begin(), fn);
        if (signature != nullptr) newDecls.insert(newDecls.begin(), signature);
    }

    node->setStatements(newDecls);
}

std::shared_ptr<syntax_tree::ASTNode> IRGenerator::buildFunction(const std::string &name, 
                                                                    const std::vector<std::shared_ptr<syntax_tree::ASTNode>> &decls)
{
    size_t arity = getArity(decls);

    auto params = generateParams(arity);

    auto caseExpr = buildCase(params, decls);

    auto lambda = buildLambda(params, caseExpr);

    auto fn = std::make_shared<syntax_tree::ASTNode>("DEF");

    fn->addStatement(
        std::make_shared<syntax_tree::Identifier>("IDENTIFIER", name)
    );

    fn->addStatement(lambda);

    return fn;
}

size_t IRGenerator::getArity(std::vector<std::shared_ptr<syntax_tree::ASTNode>> decls) {
    auto& decl = decls[0];
    if (decl->getNodeType() == "SIGNATURE") {
        return decl->getStatement(1)->getStatementCount()-1;
    }
    else if (decl->getNodeType() == "DEF" || decl->getNodeType() == "=") {
        if (decl->getStatement(0)->getStatementCount() > 0) 
            return decl->getStatement(0)->getStatement(0)->getStatementCount();
    }
    return 0;
}

std::vector<std::shared_ptr<syntax_tree::ASTNode>> IRGenerator::generateParams(size_t arity)
{
    std::vector<std::shared_ptr<syntax_tree::ASTNode>> params;

    for (size_t i = 0; i < arity; ++i)
    {
        auto name = "v" + std::to_string(i+1);

        params.push_back(
            std::make_shared<syntax_tree::Identifier>(
                "IDENTIFIER",
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
                c.patterns = d->getStatement(0)->getStatement(0)->getStatements();
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
        auto lambda = std::make_shared<ASTNode>("λ");

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

    auto caseNode = std::make_shared<ASTNode>("CASE");
    auto tuple = std::make_shared<ASTNode>("TUPLE");
    tuple->setStatements(vars);

    caseNode->addStatement(tuple);

    auto alts = std::make_shared<ASTNode>("ALTS");
    for (auto& c : clauses)
    {
        auto alt = std::make_shared<ASTNode>("ALT");

        auto patterns_tuple = std::make_shared<ASTNode>("TUPLE_PATTERN");
        patterns_tuple->setStatements(c.patterns);

        alt->addStatement(patterns_tuple);
        alt->addStatement(c.body);

        alts->addStatement(alt);
    }

    caseNode->addStatement(alts);

    return caseNode;
}