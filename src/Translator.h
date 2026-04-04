#pragma once

#include "AST.h"

class Translator {
public:
    Translator() = default;

    syntax_tree::AST translate(syntax_tree::AST ir);

private:
    void translateNode                (std::shared_ptr<syntax_tree::ASTNode> node, int index,
                                       std::shared_ptr<syntax_tree::ASTNode> parent);

    void translateListComprehension   (std::shared_ptr<syntax_tree::ASTNode> node, int index, 
                                       std::shared_ptr<syntax_tree::ASTNode> parent);
    void translateCase                (std::shared_ptr<syntax_tree::ASTNode> node);
    std::shared_ptr<syntax_tree::ASTNode> translateLetWhere (std::shared_ptr<syntax_tree::ASTNode> e, 
                                                             std::vector<std::shared_ptr<syntax_tree::ASTNode>> defs);
    void translateIf                  (std::shared_ptr<syntax_tree::ASTNode> node);
    void translateList                (std::shared_ptr<syntax_tree::ASTNode> node);
    void translateListHeadTailPattern (std::shared_ptr<syntax_tree::ASTNode> node);
    void translateString              (std::shared_ptr<syntax_tree::ASTNode> node, int index, 
                                       std::shared_ptr<syntax_tree::ASTNode> parent);
    void translateChar                (std::shared_ptr<syntax_tree::ASTNode> node, int index, 
                                       std::shared_ptr<syntax_tree::ASTNode> parent);

    std::shared_ptr<syntax_tree::ASTNode> revomeFirstQualifier(std::shared_ptr<syntax_tree::ASTNode> node, std::string t = "");
};
