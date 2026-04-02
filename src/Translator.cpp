#include "Translator.h"

syntax_tree::AST Translator::translate(syntax_tree::AST ir) {
    translateNode(ir.getRoot());
    return ir;
}

void Translator::translateNode(std::shared_ptr<syntax_tree::ASTNode> node) {
    if (node->getNodeType() == "LIST_NODE" || node->getNodeType() == "LIST_PATTERN") {
        translateList(node);
    }
    if (node->getNodeType() == "LIST_HEAD_TAIL_PATTERN") {
        translateListHeadTailPattern(node);
    }
    if (node->getNodeType() == "IF") {
        translateIf(node);
    }
    if (node->getNodeType() == "CASE") {
        translateCase(node);
    }

    for (auto& n : node->getStatements()) {
        translateNode(n);
    }
}

void Translator::translateListComprehension(std::shared_ptr<syntax_tree::ASTNode> node) {}

void Translator::translateCase(std::shared_ptr<syntax_tree::ASTNode> node) {
    auto expr = node->getStatement(0);
    auto alts = node->getStatement(1)->getStatements();

    auto fatbar = std::make_shared<syntax_tree::Fatbar>("FATBAR");
    for (int i = 0; i < alts.size(); i++) {
        auto lambda = std::make_shared<syntax_tree::Lambda>("λ");
        lambda->setStatements(alts[i]->getStatements());

        auto call = std::make_shared<syntax_tree::Call>("CALL");
        call->addStatement(lambda);
        call->addStatement(expr);
        
        fatbar->addStatement(call);
        fatbar->addStatement(std::make_shared<syntax_tree::ASTNode>("[]"));
    }
    auto err = std::make_shared<syntax_tree::ASTNode>("ERROR");
    fatbar->addStatement(err);

    node->clearStatements();
    node->setNodeType("FATBAR");
    node->setStatements(fatbar->getStatements());
}

void Translator::translateLet(std::shared_ptr<syntax_tree::ASTNode> node) {}

void Translator::translateWhere(std::shared_ptr<syntax_tree::ASTNode> node) {}

void Translator::translateIf(std::shared_ptr<syntax_tree::ASTNode> node) {
    auto alts = std::make_shared<syntax_tree::ASTNode>("ALTS");
    auto alt1 = std::make_shared<syntax_tree::ASTNode>("ALT");
    auto alt2 = std::make_shared<syntax_tree::ASTNode>("ALT");
    auto p1 = std::make_shared<syntax_tree::Constructor>("CONSTRUCTOR_PATTERN");
    auto p2 = std::make_shared<syntax_tree::Constructor>("CONSTRUCTOR_PATTERN");
    auto t = std::make_shared<syntax_tree::LiteralTypeConstructor>("LiteralTypeConstructor", "True");
    auto f = std::make_shared<syntax_tree::LiteralTypeConstructor>("LiteralTypeConstructor", "False");
    
    p1->addStatement(t);
    p2->addStatement(f);

    alt1->addStatement(p1);
    alt1->addStatement(node->getStatement(1));
    alt2->addStatement(p2);
    alt2->addStatement(node->getStatement(2));

    alts->addStatement(alt1);
    alts->addStatement(alt2);

    auto expr = node->getStatement(0);
    node->setNodeType("CASE");
    node->clearStatements();
    node->addStatement(expr);
    node->addStatement(alts);
}

void Translator::translateList(std::shared_ptr<syntax_tree::ASTNode> node) {
    auto stmts = node->getStatements();

    auto tmp = std::make_shared<syntax_tree::Operator>(":");
    tmp->addStatement(stmts[stmts.size()-1]);
    tmp->addStatement(std::make_shared<syntax_tree::LiteralNil>("NIL"));

    for (int i = stmts.size()-2; i >= 0; i--) {
        auto cons = std::make_shared<syntax_tree::Operator>(":");
        cons->addStatement(stmts[i]);
        cons->addStatement(tmp);
        tmp = cons;
    }

    node->clearStatements();
    node->setNodeType(":");
    node->addStatements(tmp->getStatements());
}

void Translator::translateListHeadTailPattern(std::shared_ptr<syntax_tree::ASTNode> node) {
    node->setNodeType(":");
}