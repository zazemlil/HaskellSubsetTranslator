#pragma once
#include "AST.h"
#include <unordered_map>

class StaticAnalyzer {
public:
    void analyze(std::shared_ptr<syntax_tree::ASTNode> root);

private:
    std::unordered_map<std::string, std::vector<std::shared_ptr<syntax_tree::ASTNode>>> groupByName(const std::vector<std::shared_ptr<syntax_tree::ASTNode>>& decls);

    void checkContiguity(const std::vector<std::shared_ptr<syntax_tree::ASTNode>>& decls);
    void checkArity(const std::vector<std::shared_ptr<syntax_tree::ASTNode>>& decls);
    void checkPatternRedundancy(const std::vector<std::shared_ptr<syntax_tree::ASTNode>>& decls);

    void analyzeNode(std::shared_ptr<syntax_tree::ASTNode> node);
};
