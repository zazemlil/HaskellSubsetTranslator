#pragma once
#include <memory>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#include "cBigNumber/Cbignum.h"
#include "cBigNumber/Cbignums.h"

namespace syntax_tree {

class LiteralInt;
class LiteralFloat;
class LiteralBoolean;
class LiteralString;
class LiteralTypeConstructor;
class ListNode;
class LiteralNil;

class ASTNode : public std::enable_shared_from_this<ASTNode> {
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

    virtual void printValue(std::ostream& os = std::cout) const { os << node_type; }

    void print(int indent = 0, std::ostream& os = std::cout) const {
        std::string indentStr = ""; 
        for (int i = 0; i < indent-1; i++) {indentStr += "    ";} 
        
        os << indentStr << "";
        this->printValue(os);
        os << '\n';
        
        for (const auto& stmt : statements) {
            stmt->print(indent + 2, os);
        }
    }

    void printFlatStack(int depth = 0, std::ostream& os = std::cout) const {
        this->printValue(os);
        
        for (const auto& stmt : statements) {
            os << "\n\t";
            stmt->printFlat(depth + 1, os);
        }
    }

    void printRecFlatStack(int deep, int maxDeep, int depth = 0, std::ostream& os = std::cout) const {
        this->printValue(os);
        
        for (const auto& stmt : statements) {
            os << "\n\t";
            stmt->printRecFlat(deep+1, maxDeep, depth + 1, os);
        }
    }

    virtual void printRecFlat(int deep, int maxDeep, int depth = 0, std::ostream& os = std::cout) {
        if (!statements.empty()) {
            os << "(";
        }
        
        printValue(os);
        
        if (!statements.empty()) {
            if (deep <= maxDeep) {
                for (const auto& stmt : statements) {
                    os << " ";
                    stmt->printRecFlat(deep+1, maxDeep, depth + 1, os);
                }
                os << ")";
            }
            else { os << " @ "; }
        }
    }

    virtual void printFlat(int depth = 0, std::ostream& os = std::cout) {
        if (!statements.empty()) {
            os << "(";
        }
        
        printValue(os);
        
        if (!statements.empty()) {
            for (const auto& stmt : statements) {
                os << " ";
                stmt->printFlat(depth + 1, os);
            }
            os << ")";
        }
    }

    std::shared_ptr<ASTNode> car();
    std::shared_ptr<ASTNode> cdr();
    std::shared_ptr<ListNode> cons(std::shared_ptr<ASTNode> node);
};


class AST {
private:
    std::shared_ptr<ASTNode> root = nullptr;

public:
    AST() {}
    AST(std::shared_ptr<ASTNode> rootNode) : root(rootNode) {}

    std::shared_ptr<ASTNode> getRoot() { return root; }
    bool isEmpty() const { return root == nullptr; }

    void print(bool flat = false, std::ostream& os = std::cout) const {
        if (root) {
            if (!flat) root->print(0);
            else root->printFlat(0, os);
            std::cout << "\n";
        } else {
            std::cout << "AST is empty.\n\n";
        }
    }
};


class LiteralInt : public ASTNode {
    cBigNumber value;
public:
    void printValue(std::ostream& os = std::cout) const override { os << value; }
    cBigNumber getValue() { return value; }
    LiteralInt(std::string t, cBigNumber v) : ASTNode(t), value(v) {}
};

class LiteralFloat : public ASTNode {
    float value;
public:
    void printValue(std::ostream& os = std::cout) const override { os << value; }
    float getValue() { return value; }
    LiteralFloat(std::string t, float v) : ASTNode(t), value(v) {}
};

class LiteralBoolean : public ASTNode {
    bool value;
public:
    void printValue(std::ostream& os = std::cout) const override {
        if (value) { os << "TRUE"; }
        else { os << "FALSE"; }
    }
    bool getValue() { return value; }
    LiteralBoolean(std::string t, bool v) : ASTNode(t), value(v) {}
};

class LiteralString : public ASTNode {
    std::string value;
public:
    void printValue(std::ostream& os = std::cout) const override { os << "\"" << value << "\""; }
    std::string getValue() { return value; }
    LiteralString(std::string t, std::string v) : ASTNode(t), value(v) {}
};

class LiteralTypeConstructor : public ASTNode {
    std::string value;
public:
    void printValue(std::ostream& os = std::cout) const override { os << value; }
    std::string getValue() { return value; }
    LiteralTypeConstructor(std::string t, std::string v) : ASTNode(t), value(v) {}
};

class ListNode : public ASTNode { 
public: 
    ListNode(std::string t) : ASTNode(t) {}
    void printFlat(int depth = 0, std::ostream& os = std::cout) override {
        os << "(";
        if (!getStatements().empty()) {
            for (const auto& stmt : getStatements()) {
                os << " ";
                stmt->printFlat(depth, os);
            }
        }
        os << ")";
    }
    void printRecFlat(int deep, int maxDeep, int depth = 0, std::ostream& os = std::cout) override {
        os << "(";
        if (!getStatements().empty()) {
            if (deep <= maxDeep) {
                for (const auto& stmt : getStatements()) {
                    os << " ";
                    stmt->printRecFlat(deep+1, maxDeep, depth + 1, os);
                }
            }
            else { os << " @ "; }
        }
        os << ")";
    }
};

class LiteralNil : public ASTNode { public: LiteralNil(std::string t) : ASTNode(t) {} };

class Identifier : public ASTNode {
    std::string value;
public:
    void printValue(std::ostream& os = std::cout) const override { os << value; }
    std::string getValue() { return value; }
    Identifier(std::string t, std::string v) : ASTNode(t), value(v) {}
};

inline std::shared_ptr<ASTNode> ASTNode::car() {
    if (this->getStatementCount() > 0 || this->getNodeType() == "NIL") {
        if (this->getNodeType() == "NIL") return shared_from_this();
        return this->getStatement(0);
    }
    throw std::runtime_error("Car error: arg must be Nil or List");
}

inline std::shared_ptr<ASTNode> ASTNode::cdr() {
    if (this->getStatementCount() > 0 || this->getNodeType() == "NIL") {
        if (this->getStatementCount() <= 1) {
            return std::make_shared<LiteralNil>("NIL");
        }
        std::shared_ptr<ASTNode> l = std::make_shared<ListNode>("LIST");
        for (size_t i = 1; i < this->getStatementCount(); i++) {
            l->addStatement(this->getStatement(i));
        }
        return l;
    }

    throw std::runtime_error("Cdr error: second param must be List or Nil");
}

inline std::shared_ptr<ListNode> ASTNode::cons(std::shared_ptr<ASTNode> node) {
    if (node->getStatementCount() > 0 || node->getNodeType() == "NIL") {
        std::shared_ptr<ListNode> l = std::make_shared<ListNode>("LIST");
    
        l->addStatement(shared_from_this());
        
        l->addStatements(node->getStatements());
        return l;
    }
    
    throw std::runtime_error("Cons error: second param must be List or Nil");
}

};