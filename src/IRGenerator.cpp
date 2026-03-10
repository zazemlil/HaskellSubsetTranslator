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

    //auto lambda = buildLambda(params, caseExpr);

    auto fn = std::make_shared<syntax_tree::ASTNode>("FUNCTION");

    fn->addStatement(
        std::make_shared<syntax_tree::Identifier>("IDENTIFIER", name)
    );

    //fn->addStatement(lambda);

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
        auto name = "v" + std::to_string(i);

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

std::shared_ptr<syntax_tree::ASTNode> IRGenerator::compileMatch(std::vector<std::shared_ptr<syntax_tree::ASTNode>> vars, 
                                                                std::vector<Clause> clauses)
{
    using namespace syntax_tree;

    if (vars.empty())
    {
        return clauses[0].body;
    }

    auto var = vars[0];

    auto caseNode = std::make_shared<ASTNode>("CASE");
    caseNode->addStatement(var);

    std::unordered_map<std::string, std::vector<Clause>> groups;

    for (auto& c : clauses)
    {
        auto p = c.patterns[0];

        std::string key = getPatternTag(p);

        groups[key].push_back(c);
    }

    for (auto& [tag, gclauses] : groups)
    {
        auto branch = std::make_shared<ASTNode>("BRANCH");

        auto pattern = gclauses[0].patterns[0];

        std::vector<Clause> newClauses;

        for (auto& cl : gclauses)
        {
            Clause nc;

            nc.body = cl.body;

            for (size_t i = 1; i < cl.patterns.size(); ++i)
                nc.patterns.push_back(cl.patterns[i]);

            newClauses.push_back(nc);
        }

        std::vector<std::shared_ptr<ASTNode>> newVars(vars.begin()+1, vars.end());

        auto expr = compileMatch(newVars, newClauses);

        branch->addStatement(pattern);
        branch->addStatement(expr);

        caseNode->addStatement(branch);
    }

    return caseNode;
}

std::string IRGenerator::getPatternTag(std::shared_ptr<syntax_tree::ASTNode> p)
{
    return std::string();
}
