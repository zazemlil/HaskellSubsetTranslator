#pragma once
#include "AST.h"

class StaticAnalyzer {
public:
    void analyze(std::shared_ptr<syntax_tree::ASTNode> root);

private:
    void checkContiguity(const std::vector<std::shared_ptr<syntax_tree::ASTNode>>& decls);
    void checkArity(const std::vector<std::shared_ptr<syntax_tree::ASTNode>>& decls);
    void checkPatternRedundancy(const std::vector<std::shared_ptr<syntax_tree::ASTNode>>& decls);

    void analyzeNode(std::shared_ptr<syntax_tree::ASTNode> node);
};
