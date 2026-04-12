#include "ScopeChecker.h"

ScopeChecker::ScopeChecker() {
    enterScope(); 
}

void ScopeChecker::enterScope() {
    symbolStack.emplace_back();
}

void ScopeChecker::exitScope() {
    if (!symbolStack.empty()) {
        symbolStack.pop_back();
    }
}

void ScopeChecker::declareSymbol(const std::string& name) {
    if (!symbolStack.empty()) {
        symbolStack.back().insert(name);
    }
}

bool ScopeChecker::isDeclared(const std::string& name) {
    for (auto it = symbolStack.rbegin(); it != symbolStack.rend(); ++it) {
        if (it->find(name) != it->end()) {
            return true;
        }
    }
    return false;
}

void ScopeChecker::collectPatterns(std::shared_ptr<syntax_tree::ASTNode> node) {
    if (!node) return;
    
    if (node->getNodeType() == "Identifier") {
        auto idNode = std::dynamic_pointer_cast<syntax_tree::Identifier>(node);
        declareSymbol(idNode->getValue());
    }
    // Здесь также стоит добавить рекурсивный обход для TUPLE_PATTERN, LIST_PATTERN и т.д.
    for (auto& child : node->getStatements()) {
        collectPatterns(child);
    }
}

void ScopeChecker::analyze(std::shared_ptr<syntax_tree::ASTNode> root) {
    if (!root) return;
    analyzeNode(root);
}

void ScopeChecker::analyzeNode(std::shared_ptr<syntax_tree::ASTNode> node) {
    if (!node) return;
    std::string type = node->getNodeType();

    if (type == "Identifier") {
        auto idNode = std::dynamic_pointer_cast<syntax_tree::Identifier>(node);
        std::string name = idNode->getValue();
        if (!isDeclared(name)) {
            throw std::runtime_error("Undefined variable: " + name);
        }
        return;
    }

    if (type == "DEFINITIONS" || type == "LET" || type == "WHERE" || type == "LC_LET") {
        enterScope();
        
        // Проход 1: Собираем все имена функций/переменных в текущем скоупе
        std::vector<std::shared_ptr<syntax_tree::ASTNode>> decls;
        if (type == "DEFINITIONS" || type == "LC_LET") decls = node->getStatements();
        else if (type == "LET") decls = node->getStatement(0)->getStatements();
        else if (type == "WHERE") decls = node->getStatement(1)->getStatements();

        for (auto& decl : decls) {
            if (decl->getNodeType() == "DEF" || decl->getNodeType() == "=") {
                 // Левая часть выражения содержит определяемое имя
                 collectPatterns(decl->getStatement(0));
            }
        }

        // Проход 2: Проверяем тела
        for (auto& child : node->getStatements()) {
            analyzeNode(child);
        }

        exitScope();
        return;
    }

    if (type == "λ") { // Лямбда-выражение
        enterScope();
        // У лямбды первый аргумент - паттерн, второй - тело
        collectPatterns(node->getStatement(0));
        analyzeNode(node->getStatement(1));
        exitScope();
        return;
    }

    if (type == "LIST_COMPREHENSION") {
        enterScope();
        
        // Обходим квалификаторы (Child 1) слева направо
        auto& qualifiers = node->getStatement(1)->getStatements();
        for (auto& qualifier : qualifiers) {
            if (qualifier->getNodeType() == "<-") {
                // Сначала анализируем правую часть (источник списка), 
                // так как она НЕ видит переменные из левой части текущего генератора.
                analyzeNode(qualifier->getStatement(1));
                
                // Затем собираем паттерны из левой части (добавляем их в Scope),
                // чтобы они были доступны для последующих квалификаторов и головного выражения.
                collectPatterns(qualifier->getStatement(0));
            } else {
                // Если это guard (охранное выражение), просто проверяем его.
                analyzeNode(qualifier);
            }
        }
        
        // Теперь, когда все переменные генераторов находятся в Scope, 
        // мы можем безопасно проверить головное выражение (Child 0).
        analyzeNode(node->getStatement(0));
        
        exitScope();
        return;
    }

    for (auto& child : node->getStatements()) {
        analyzeNode(child);
    }
}