#pragma once
#include <memory>
#include <vector>
#include <string>
#include <iostream>

namespace syntax_tree {

class ASTNode {
private:
    std::string text;
    std::vector<std::unique_ptr<ASTNode>> statements; 

public:
    ASTNode() {}
    ASTNode(std::string t) : text(t) {}

    void addStatement(std::unique_ptr<ASTNode> stmt) {
        statements.push_back(std::move(stmt));
    }
    
    size_t getStatementCount() const { 
        return statements.size(); 
    }

    ASTNode* getStatement(size_t index) const { 
        return statements.at(index).get(); 
    }

    void print(int indent = 0) const {
        std::string indentStr = "|";
        for (int i = 0; i < indent-1; i++) {indentStr += "    |";}
        
        std::cout << indentStr << "--- " << text << "\n";
        
        for (const auto& stmt : statements) {
            stmt->print(indent + 2);
        }
    }
};


class AST {
private:
    std::unique_ptr<ASTNode> root = nullptr;

public:
    AST() {}
    AST(std::unique_ptr<ASTNode> rootNode) : root(std::move(rootNode)) {}

    bool isEmpty() {
        if (root == nullptr) return true;
        return false;
    }

    void print() const {
        if (root) {
            root->print(0);
        } else {
            std::cout << "AST is empty.\n";
        }
    }
};

};

