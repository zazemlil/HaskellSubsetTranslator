#pragma once
#include "AST.h"
#include "IRGenerator.h"
#include "GroupingService.h"

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
    IRGenerator* getIRGenerator();
    StaticAnalyzer();
    ~StaticAnalyzer();

private:
    IRGenerator* generator;

    void checkContiguity(const std::vector<std::shared_ptr<syntax_tree::ASTNode>>& decls);
    void checkArity(const std::vector<std::shared_ptr<syntax_tree::ASTNode>>& decls);
    size_t getLambdaAbstractionArity(const std::shared_ptr<syntax_tree::ASTNode>& node);
    bool isPattern(const std::shared_ptr<syntax_tree::ASTNode>& node);

    PatternKind getPatternKind(std::shared_ptr<syntax_tree::ASTNode> p);
    std::vector<std::shared_ptr<syntax_tree::ASTNode>> extractPatterns(std::shared_ptr<syntax_tree::ASTNode> decl);
    bool patternCovers(std::shared_ptr<syntax_tree::ASTNode> p1, std::shared_ptr<syntax_tree::ASTNode> p2);
    bool rowCovers(const std::vector<std::shared_ptr<syntax_tree::ASTNode>>& r1, const std::vector<std::shared_ptr<syntax_tree::ASTNode>>& r2);
    void checkPatternRedundancy(std::string name, const std::vector<std::shared_ptr<syntax_tree::ASTNode>>& decls);

    void analyzeNode(std::shared_ptr<syntax_tree::ASTNode> node);
};
