#pragma once
#include "AST.h"

class IRGenerator {
public:
    std::shared_ptr<syntax_tree::ASTNode> generate(std::shared_ptr<syntax_tree::ASTNode> root);

private:
    std::shared_ptr<syntax_tree::ASTNode> transformNode(std::shared_ptr<syntax_tree::ASTNode> node);

    std::shared_ptr<syntax_tree::ASTNode> desugarFunction(
        std::vector<std::shared_ptr<syntax_tree::ASTNode>>& equations
    );
};
