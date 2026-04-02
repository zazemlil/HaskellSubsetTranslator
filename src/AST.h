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
class LiteralString;
class LiteralTypeConstructor;
class LiteralNil;

class ASTNode : public std::enable_shared_from_this<ASTNode> {
    std::string node_type;
    std::vector<std::shared_ptr<ASTNode>> statements;

public:
    ASTNode() {}
    ASTNode(std::string t) : node_type(t) {}
    
    virtual ~ASTNode() = default;

    std::string getNodeType() const { return node_type; }
    void setNodeType(std::string t) { this->node_type = t; }
    size_t getStatementCount() const { return statements.size(); }
    void clearStatements() { statements = std::vector<std::shared_ptr<ASTNode>>(); }

    std::shared_ptr<ASTNode>& getStatement(size_t index) { return statements.at(index); }
    void addStatement(std::shared_ptr<ASTNode> stmt) { statements.push_back(stmt); }
    void addStatementFront(std::shared_ptr<ASTNode> stmt) { statements.insert(statements.begin(), stmt); }
    
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

    virtual void printFlat(int depth = 0, std::ostream& os = std::cout) {
        printValue(os);
        
        if (!statements.empty()) {
            for (const auto& stmt : statements) {
                os << " ";
                stmt->printFlat(depth + 1, os);
            }
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

    void print(bool flat = false, std::ostream& os = std::cout) const {
        if (root) {
            if (!flat) root->print(0);
            else root->printFlat(0, os);
            std::cout << "\n";
        } else {
            os << "AST is empty.\n\n";
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

class Tuple : public ASTNode { 
public: 
    Tuple(std::string t) : ASTNode(t) {}
    void printFlat(int depth = 0, std::ostream& os = std::cout) override {
        os << "(Tuple)";
        for (const auto& stmt : getStatements()) {
            os << " ";
            std::string t = stmt->getNodeType();
            if (t == "Identifier" || t == "LiteralInt" || t == "LiteralFloat" || t == "LiteralString" || t == "LIST" || t == "NIL" || stmt->getStatementCount() == 1) {
                stmt->printFlat(depth, os);
            }
            else {
                os << "(";
                stmt->printFlat(depth, os);
                os << ")";
            }
        }
    }
};

class LiteralNil : public ASTNode { 
public: 
    LiteralNil(std::string t) : ASTNode(t) {} 
    void printFlat(int depth = 0, std::ostream& os = std::cout) override {
        os << "Nil";
    }
};

class Identifier : public ASTNode {
    std::string value;
public:
    void printValue(std::ostream& os = std::cout) const override { os << value; }
    std::string getValue() { return value; }
    Identifier(std::string t, std::string v) : ASTNode(t), value(v) {}
};

class Definition : public ASTNode { 
public: 
    Definition(std::string t) : ASTNode(t) {}
    void printFlat(int depth = 0, std::ostream& os = std::cout) override {
        auto& stmts = getStatements();
        
        stmts[0]->printFlat(depth, os);
        os << " = ";
        stmts[1]->printFlat(depth, os);
    }
};

class Lambda : public ASTNode { 
public: 
    Lambda(std::string t) : ASTNode(t) {}
    void printFlat(int depth = 0, std::ostream& os = std::cout) override {
        auto& stmts = getStatements();
        this->printValue(os);
        if (stmts[0]->getNodeType() == ":" || stmts[0]->getNodeType() == "TUPLE" || stmts[0]->getNodeType() == "TUPLE_PATTERN") {
            os << "(";
            stmts[0]->printFlat(depth, os);
            os << ")";
        }
        else {
            stmts[0]->printFlat(depth, os);
        }
        os << ".";
        stmts[1]->printFlat(depth, os);
    }
};

class Constructor : public ASTNode { 
public: 
    Constructor(std::string t) : ASTNode(t) {}
    void printFlat(int depth = 0, std::ostream& os = std::cout) override {
        auto& stmts = getStatements();
        if (getStatementCount() == 1) {
            stmts[0]->printFlat(depth, os);
            return;
        }
        os << "(";
        stmts[0]->printFlat(depth, os);
        os << ")";
        for (int i = 1; i < stmts.size(); i++) {
            os << " ";
            std::string t = stmts[i]->getNodeType();
            if (t == "Identifier" || t == "LiteralInt" || t == "LiteralFloat" || t == "LiteralString" || t == "LIST" || t == "NIL" || stmts[i]->getStatementCount() == 1) {
                stmts[i]->printFlat(depth, os);
            }
            else {
                os << "(";
                stmts[i]->printFlat(depth, os);
                os << ")";
            }
        }
    }
};

class Call : public ASTNode { 
public: 
    Call(std::string t) : ASTNode(t) {}
    void printFlat(int depth = 0, std::ostream& os = std::cout) override {
        auto& stmts = getStatements();
        os << "(";
        stmts[0]->printFlat(depth, os);
        os << ")";
        for (int i = 1; i < stmts.size(); i++) {
            os << " ";
            std::string t = stmts[i]->getNodeType();
            if (t == "Identifier" || t == "LiteralInt" || t == "LiteralFloat" || t == "LiteralString" || t == "LIST" || t == "NIL" || stmts[i]->getStatementCount() == 1) {
                stmts[i]->printFlat(depth, os);
            }
            else {
                os << "(";
                stmts[i]->printFlat(depth, os);
                os << ")";
            }
        }
    }
};

class Operator : public ASTNode { 
public: 
    Operator(std::string t) : ASTNode(t) {}
    void printFlat(int depth = 0, std::ostream& os = std::cout) override {
        auto& stmts = getStatements();
        os << "(";
        this->printValue(os);
        os << ")";
        for (int i = 0; i < stmts.size(); i++) {
            os << " ";
            std::string t = stmts[i]->getNodeType();
            if (t == "Identifier" || t == "LiteralInt" || t == "LiteralFloat" || t == "LiteralString" || t == "LIST" || t == "NIL" || t == "_") {
                stmts[i]->printFlat(depth, os);
            }
            else {
                os << "(";
                stmts[i]->printFlat(depth, os);
                os << ")";
            }
        }
    }
};

class Fatbar : public ASTNode { 
public: 
    Fatbar(std::string t) : ASTNode(t) {}
    void printFlat(int depth = 0, std::ostream& os = std::cout) override {
        auto& stmts = getStatements();
        os << "( ";
        for (int i = 0; i < stmts.size(); i++) {
            std::string t = stmts[i]->getNodeType();
            if (t == "ERROR") {
                stmts[i]->printFlat(depth, os);
            }
            else if (t == "[]") {
                os << " ";
                stmts[i]->printFlat(depth, os);
                os << " ";
            }
            else {
                os << "(";
                stmts[i]->printFlat(depth, os);
                os << ")";
            }
        }
        os << ") ";
    }
};

};