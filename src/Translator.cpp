#include "Translator.h"

syntax_tree::AST Translator::translate(syntax_tree::AST ir) {
    return syntax_tree::AST(translateNode(ir.getRoot()));
}

std::shared_ptr<syntax_tree::ASTNode> Translator::translateNode(std::shared_ptr<syntax_tree::ASTNode> node) {
    if (node->getNodeType() == "IF") {
        translateIf(node);
    }


    for (auto& n : node->getStatements()) {
        translateNode(n);
    }
    return node;
}

std::shared_ptr<syntax_tree::ASTNode> Translator::translateLambda(std::shared_ptr<syntax_tree::ASTNode> node)
{
    return std::shared_ptr<syntax_tree::ASTNode>();
}

std::shared_ptr<syntax_tree::ASTNode> Translator::translateListComprehension(std::shared_ptr<syntax_tree::ASTNode> node)
{
    return std::shared_ptr<syntax_tree::ASTNode>();
}

std::shared_ptr<syntax_tree::ASTNode> Translator::translateApplication(std::shared_ptr<syntax_tree::ASTNode> node)
{
    return std::shared_ptr<syntax_tree::ASTNode>();
}

std::shared_ptr<syntax_tree::ASTNode> Translator::translateCase(std::shared_ptr<syntax_tree::ASTNode> node)
{
    return std::shared_ptr<syntax_tree::ASTNode>();
}

std::shared_ptr<syntax_tree::ASTNode> Translator::translateLet(std::shared_ptr<syntax_tree::ASTNode> node)
{
    return std::shared_ptr<syntax_tree::ASTNode>();
}

std::shared_ptr<syntax_tree::ASTNode> Translator::translateWhere(std::shared_ptr<syntax_tree::ASTNode> node)
{
    return std::shared_ptr<syntax_tree::ASTNode>();
}

std::shared_ptr<syntax_tree::ASTNode> Translator::translateIf(std::shared_ptr<syntax_tree::ASTNode> node) {
    auto alts = std::make_shared<syntax_tree::ASTNode>("ALTS");
    auto alt1 = std::make_shared<syntax_tree::ASTNode>("ALT");
    auto alt2 = std::make_shared<syntax_tree::ASTNode>("ALT");
    auto p1 = std::make_shared<syntax_tree::Constructor>("CONSTRUCTOR_PATTERN");
    auto p2 = std::make_shared<syntax_tree::Constructor>("CONSTRUCTOR_PATTERN");
    auto t = std::make_shared<syntax_tree::LiteralTypeConstructor>("LiteralTypeConstructor", "True");
    auto f = std::make_shared<syntax_tree::LiteralTypeConstructor>("LiteralTypeConstructor", "False");
    
    p1->addStatement(t);
    p2->addStatement(f);

    alt1->addStatement(p1);
    alt1->addStatement(node->getStatement(1));
    alt2->addStatement(p2);
    alt2->addStatement(node->getStatement(2));

    alts->addStatement(alt1);
    alts->addStatement(alt2);

    auto expr = node->getStatement(0);
    node->setNodeType("CASE");
    node->clearStatements();
    node->addStatement(expr);
    node->addStatement(alts);
    
    return node;
}

std::shared_ptr<syntax_tree::ASTNode> Translator::translateList(std::shared_ptr<syntax_tree::ASTNode> node)
{
    return std::shared_ptr<syntax_tree::ASTNode>();
}

std::shared_ptr<syntax_tree::ASTNode> Translator::translateListPattern(std::shared_ptr<syntax_tree::ASTNode> node)
{
    return std::shared_ptr<syntax_tree::ASTNode>();
}

std::shared_ptr<syntax_tree::ASTNode> Translator::translateListHeadTailPattern(std::shared_ptr<syntax_tree::ASTNode> node)
{
    return std::shared_ptr<syntax_tree::ASTNode>();
}

std::shared_ptr<syntax_tree::ASTNode> Translator::translateLiteral(std::shared_ptr<syntax_tree::ASTNode> node)
{
    return std::shared_ptr<syntax_tree::ASTNode>();
}

std::shared_ptr<syntax_tree::ASTNode> Translator::translateIdentifier(std::shared_ptr<syntax_tree::ASTNode> node)
{
    return std::shared_ptr<syntax_tree::ASTNode>();
}

std::shared_ptr<syntax_tree::ASTNode> Translator::translateConstructor(std::shared_ptr<syntax_tree::ASTNode> node)
{
    return std::shared_ptr<syntax_tree::ASTNode>();
}
