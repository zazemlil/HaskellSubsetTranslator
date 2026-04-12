#pragma once
#include "AST.h"

#include <unordered_set>
#include <stdexcept>

class ScopeChecker {
private:
    // Стек областей видимости. 
    // Каждая область - множество объявленных идентификаторов.
    std::vector<std::unordered_set<std::string>> symbolStack;

    void enterScope();
    void exitScope();
    void declareSymbol(const std::string& name);
    bool isDeclared(const std::string& name);

    void analyzeNode(std::shared_ptr<syntax_tree::ASTNode> node);
    
    // Вспомогательный метод для сбора паттернов (переменных)
    void collectPatterns(std::shared_ptr<syntax_tree::ASTNode> patternNode);

public:
    ScopeChecker();
    void analyze(std::shared_ptr<syntax_tree::ASTNode> root);
};