#pragma once
#include <memory>
#include <vector>
#include <string>
#include <iostream>

namespace syntax_tree {

class ASTNode {
    std::string node_type;
    std::vector<std::shared_ptr<ASTNode>> statements;

public:
    ASTNode() {}
    ASTNode(std::string t) : node_type(t) {}
    
    virtual ~ASTNode() = default;

    std::string getNodeType() const { return node_type; }
    size_t getStatementCount() const { return statements.size(); }

    std::shared_ptr<ASTNode>& getStatement(size_t index) { return statements.at(index); }
    void addStatement(std::shared_ptr<ASTNode> stmt) { statements.push_back(stmt); }

    void setStatements(std::vector<std::shared_ptr<ASTNode>>& new_statements) { this->statements = new_statements; }
    std::vector<std::shared_ptr<ASTNode>>& getStatements() { return statements; }
    
    void addStatements(std::vector<std::shared_ptr<ASTNode>>& new_statements) {
        this->statements.insert(
            std::end(this->statements),
            std::begin(new_statements),
            std::end(new_statements)
        );
    }

    virtual void printValue() const { std::cout << node_type; }

    void print(int indent = 0) const {
        std::string indentStr = ""; 
        for (int i = 0; i < indent-1; i++) {indentStr += "    ";} 
        
        std::cout << indentStr << "";
        this->printValue();
        std::cout << '\n';
        
        for (const auto& stmt : statements) {
            stmt->print(indent + 2);
        }
    }
};


class AST {
private:
    std::shared_ptr<ASTNode> root = nullptr;

public:
    AST() {}
    AST(std::shared_ptr<ASTNode> rootNode) : root(rootNode) {}

    std::shared_ptr<ASTNode> getRoot() { return root; }
    bool isEmpty() const { return root == nullptr; }

    void print() const {
        if (root) {
            root->print(0);
            std::cout << "\n";
        } else {
            std::cout << "AST is empty.\n\n";
        }
    }
};

};

