#pragma once

#include "AST.h"

class Translator
{
public:
    Translator() = default;

    // Основная функция трансляции
    syntax_tree::AST translate(syntax_tree::AST ir);

private:

    // Рекурсивная трансляция
    std::shared_ptr<syntax_tree::ASTNode> translateNode(std::shared_ptr<syntax_tree::ASTNode> node);

    // Специализированные трансляции
    std::shared_ptr<syntax_tree::ASTNode> translateLambda(std::shared_ptr<syntax_tree::ASTNode> node);

    std::shared_ptr<syntax_tree::ASTNode> translateListComprehension(std::shared_ptr<syntax_tree::ASTNode> node);

    std::shared_ptr<syntax_tree::ASTNode> translateApplication(std::shared_ptr<syntax_tree::ASTNode> node);

    std::shared_ptr<syntax_tree::ASTNode> translateCase(std::shared_ptr<syntax_tree::ASTNode> node);

    std::shared_ptr<syntax_tree::ASTNode> translateLet(std::shared_ptr<syntax_tree::ASTNode> node);

    std::shared_ptr<syntax_tree::ASTNode> translateWhere(std::shared_ptr<syntax_tree::ASTNode> node);

    std::shared_ptr<syntax_tree::ASTNode> translateIf(std::shared_ptr<syntax_tree::ASTNode> node);

    std::shared_ptr<syntax_tree::ASTNode> translateList(std::shared_ptr<syntax_tree::ASTNode> node);

    std::shared_ptr<syntax_tree::ASTNode> translateListPattern(std::shared_ptr<syntax_tree::ASTNode> node);

    std::shared_ptr<syntax_tree::ASTNode> translateListHeadTailPattern(std::shared_ptr<syntax_tree::ASTNode> node);

    std::shared_ptr<syntax_tree::ASTNode> translateLiteral(std::shared_ptr<syntax_tree::ASTNode> node);

    std::shared_ptr<syntax_tree::ASTNode> translateIdentifier(std::shared_ptr<syntax_tree::ASTNode> node);

    std::shared_ptr<syntax_tree::ASTNode> translateConstructor(std::shared_ptr<syntax_tree::ASTNode> node);

};
