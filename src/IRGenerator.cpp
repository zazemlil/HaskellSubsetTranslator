#include "IRGenerator.h"

std::shared_ptr<syntax_tree::ASTNode> IRGenerator::generate(std::shared_ptr<syntax_tree::ASTNode> node, std::unordered_map<std::string, std::vector<std::shared_ptr<syntax_tree::ASTNode>>> groups)
{
    std::vector<std::shared_ptr<syntax_tree::ASTNode>> newDecls;

    for (auto& [name, decls] : groups)
    {
        auto fn = buildFunction(name, decls);
        newDecls.push_back(fn);
    }

    node->setStatements(newDecls);

    return node;
}

std::shared_ptr<syntax_tree::ASTNode> IRGenerator::buildFunction(const std::string &name, const std::vector<std::shared_ptr<syntax_tree::ASTNode>> &decls)
{
    size_t arity = getArity(decls);

    auto params = generateParams(arity);

    //auto caseExpr = buildCase(params, decls);

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
    return std::vector<std::shared_ptr<syntax_tree::ASTNode>>();
}
