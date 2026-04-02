#pragma once

#include "AST.h"

class Translator {
public:
    Translator() = default;

    syntax_tree::AST translate(syntax_tree::AST ir);

private:
    void translateNode                (std::shared_ptr<syntax_tree::ASTNode> node);

    void translateListComprehension   (std::shared_ptr<syntax_tree::ASTNode> node);
    void translateCase                (std::shared_ptr<syntax_tree::ASTNode> node);
    void translateLet                 (std::shared_ptr<syntax_tree::ASTNode> node);
    void translateWhere               (std::shared_ptr<syntax_tree::ASTNode> node);
    void translateIf                  (std::shared_ptr<syntax_tree::ASTNode> node);
    void translateList                (std::shared_ptr<syntax_tree::ASTNode> node);
    void translateListHeadTailPattern (std::shared_ptr<syntax_tree::ASTNode> node);
};
