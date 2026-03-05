#pragma once
#include "AST.h"
#include <unordered_map>

enum PatternKind {
    IDENTIFIER,
    WILDCARD,
    LITERAL,
    CONSTRUCTOR,
    LIST_EMPTY,
    LIST_CONS,
    LIST_ENUM
};

class StaticAnalyzer {
public:
    void analyze(std::shared_ptr<syntax_tree::ASTNode> root);

private:
    std::unordered_map<std::string, std::vector<std::shared_ptr<syntax_tree::ASTNode>>> groupByName(const std::vector<std::shared_ptr<syntax_tree::ASTNode>>& decls);

    void checkContiguity(const std::vector<std::shared_ptr<syntax_tree::ASTNode>>& decls);
    void checkArity(const std::vector<std::shared_ptr<syntax_tree::ASTNode>>& decls);

    PatternKind getPatternKind(std::shared_ptr<syntax_tree::ASTNode> p);
    std::vector<std::shared_ptr<syntax_tree::ASTNode>> extractPatterns(std::shared_ptr<syntax_tree::ASTNode> decl);
    bool patternCovers(std::shared_ptr<syntax_tree::ASTNode> p1, std::shared_ptr<syntax_tree::ASTNode> p2);
    bool rowCovers(const std::vector<std::shared_ptr<syntax_tree::ASTNode>>& r1, const std::vector<std::shared_ptr<syntax_tree::ASTNode>>& r2);
    void checkPatternRedundancy(std::string name, const std::vector<std::shared_ptr<syntax_tree::ASTNode>>& decls);

    void analyzeNode(std::shared_ptr<syntax_tree::ASTNode> node);
};
