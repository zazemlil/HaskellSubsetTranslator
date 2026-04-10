#include "Translator.h"

syntax_tree::AST Translator::translate(syntax_tree::AST ir) {
    translateNode(ir.getRoot(), -1, nullptr);
    return ir;
}

void Translator::translateNode(std::shared_ptr<syntax_tree::ASTNode> node, int index, std::shared_ptr<syntax_tree::ASTNode> parent) {
    if (node->getNodeType() == "DEF") {
        if (parent->getNodeType() == "DEFINITIONS") {
            translateGlobalDef(node, index, parent);
        }
    }
    if (node->getNodeType() == "LIST_COMPREHENSION") {
        translateListComprehension(node, index, parent);
        node = parent->getStatement(index);
    }
    if (node->getNodeType() == "LIST_NODE" || node->getNodeType() == "LIST_PATTERN") {
        translateList(node);
    }
    if (node->getNodeType() == "LIST_HEAD_TAIL_PATTERN") {
        translateListHeadTailPattern(node);
    }
    if (node->getNodeType() == "LiteralString") {
        translateString(node, index, parent);
    }
    if (node->getNodeType() == "LiteralChar") {
        translateChar(node, index, parent);
    }
    if (node->getNodeType() == "IF") {
        translateIf(node);
    }
    if (node->getNodeType() == "CASE") {
        translateCase(node);
    }
    if (node->getNodeType() == "LET") {
        auto defs = node->getStatement(0)->getStatements();
        auto e = node->getStatement(1);
        auto n = translateLetWhere(e, defs);
        node->clearStatements();
        node->setNodeType("CALL");
        node->setStatements(n->getStatements());
    }
    if (node->getNodeType() == "WHERE") {
        auto defs = node->getStatement(1)->getStatements();
        auto e = node->getStatement(0);
        auto n = translateLetWhere(e, defs);
        node->clearStatements();
        node->setNodeType("CALL");
        node->setStatements(n->getStatements());
    }

    int i = 0;
    for (auto& n : node->getStatements()) {
        translateNode(n, i, node);
        i++;
    }
}

void Translator::translateListComprehension(std::shared_ptr<syntax_tree::ASTNode> node, int index, std::shared_ptr<syntax_tree::ASTNode> parent) {
    if (auto n = revomeFirstQualifier(node->getStatement(1), "LC_LET")) {
        auto l = std::make_shared<syntax_tree::ASTNode>("LIST");

        for (auto& d : n->getStatements()) {
            l->addStatement(d);
        }
        
        while (auto next = revomeFirstQualifier(node->getStatement(1), "LC_LET")) {
            for (auto& d : next->getStatements()) {
                l->addStatement(d);
            }
        }

        auto let = std::make_shared<syntax_tree::Call>("LET");
        let->addStatement(l);
        let->addStatement(node);

        parent->getStatement(index) = let;
    }
    else if (auto n = revomeFirstQualifier(node->getStatement(1), "<-")) {
        auto lambda = std::make_shared<syntax_tree::Lambda>("λ");
        lambda->addStatement(n->getStatement(0));
        lambda->addStatement(node);
        
        auto call = std::make_shared<syntax_tree::Call>("CALL");
        call->addStatement(std::make_shared<syntax_tree::Identifier>("Identifier", "concatMap"));
        call->addStatement(lambda);
        call->addStatement(n->getStatement(1));

        parent->getStatement(index) = call;
    }
    else if (auto n = revomeFirstQualifier(node->getStatement(1))) {
        auto ifExpr = std::make_shared<syntax_tree::Fatbar>("IF");
        ifExpr->addStatement(n);
        ifExpr->addStatement(node);
        ifExpr->addStatement(std::make_shared<syntax_tree::LiteralNil>("NIL"));
        
        parent->getStatement(index) = ifExpr;
    }
    else {
        auto list = std::make_shared<syntax_tree::Operator>("LIST_NODE");
        list->addStatement(node->getStatement(0));

        parent->getStatement(index) = list;
    }
}

void Translator::translateGlobalDef(std::shared_ptr<syntax_tree::ASTNode> node, int index, std::shared_ptr<syntax_tree::ASTNode> parent) {
    if (auto id = std::dynamic_pointer_cast<syntax_tree::Identifier>(node->getStatement(0))) {
        if (isRecursive(id->getValue(), node->getStatement(1))) {
            auto lambda = std::make_shared<syntax_tree::Lambda>("λ");
            lambda->addStatement(node->getStatement(0));
            lambda->addStatement(node->getStatement(1));

            auto call = std::make_shared<syntax_tree::Call>("CALL");
            call->addStatement(std::make_shared<syntax_tree::Identifier>("Identifier", "fix"));
            call->addStatement(lambda);

            node->getStatement(1) = call;
        }
    }
}

std::shared_ptr<syntax_tree::ASTNode> Translator::revomeFirstQualifier(std::shared_ptr<syntax_tree::ASTNode> node, std::string t) {
    auto& stmts = node->getStatements();

    for (size_t i = 0; i < stmts.size(); ++i) {
        if (stmts[i]->getNodeType() == t || t == "") {
            auto result = stmts[i];               
            stmts.erase(stmts.begin() + i);         
            return result;                          
        }
    }

    return nullptr;
}

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

std::shared_ptr<syntax_tree::ASTNode> Translator::translateLetWhere(std::shared_ptr<syntax_tree::ASTNode> e, std::vector<std::shared_ptr<syntax_tree::ASTNode>> defs) {
    auto lam1 = std::make_shared<syntax_tree::Lambda>("λ");
    auto lam2 = std::make_shared<syntax_tree::Lambda>("λ");
    
    bool isRec = false;
    if (defs.size() > 1) {
        auto tuplep1 = std::make_shared<syntax_tree::Tuple>("TUPLE_PATTERN");
        auto tuplep2 = std::make_shared<syntax_tree::Tuple>("TUPLE_PATTERN");
        auto tuple = std::make_shared<syntax_tree::Tuple>("TUPLE");
        for (auto n : defs) {
            auto id = n->getStatement(0);
            auto body = n->getStatement(1);

            auto idName = std::dynamic_pointer_cast<syntax_tree::Identifier>(id)->getValue();
            if (isRecursive(idName, body)) isRec = true;

            tuplep1->addStatement(id);
            tuplep2->addStatement(id);
            tuple->addStatement(body);
        }

        lam1->addStatement(tuplep1);
        lam1->addStatement(e);

        lam2->addStatement(tuplep2);
        lam2->addStatement(tuple);
    }
    else {
        auto id = defs[0]->getStatement(0);
        auto body = defs[0]->getStatement(1);

        auto idName = std::dynamic_pointer_cast<syntax_tree::Identifier>(id)->getValue();
        if (isRecursive(idName, body)) isRec = true;

        lam1->addStatement(id);
        lam1->addStatement(e);

        lam2->addStatement(id);
        lam2->addStatement(body);
    }
    
    auto call1 = std::make_shared<syntax_tree::Call>("CALL");
    call1->addStatement(lam1);

    if (isRec) {
        auto call2 = std::make_shared<syntax_tree::Call>("CALL");
        auto fix = std::make_shared<syntax_tree::Identifier>("Identifier", "fix");
        call2->addStatement(fix);
        call2->addStatement(lam2);

        call1->addStatement(call2);
    }
    else {
        call1->addStatement(lam2->getStatement(1));
    }
    
    return call1;
}

bool Translator::isRecursive(const std::string& name, std::shared_ptr<syntax_tree::ASTNode> node) {
    if (!node) return false;

    // 1. Проверка идентификатора
    if (node->getNodeType() == "Identifier") {
        auto id = std::dynamic_pointer_cast<syntax_tree::Identifier>(node);
        if (id && id->getValue() == name) {
            return true;
        }
    }

    // 2. Если это let — проверяем shadowing
    if (node->getNodeType() == "LET") {
        auto bindings = node->getStatement(0);

        for (auto& bind : bindings->getStatements()) {
            auto id = bind->getStatement(0);
            auto idName = std::dynamic_pointer_cast<syntax_tree::Identifier>(id)->getValue();

            if (idName == name) {
                return false; // имя затенено
            }
        }
    }

    if (node->getNodeType() == "WHERE") {
        auto bindings = node->getStatement(1);

        for (auto& bind : bindings->getStatements()) {
            auto id = bind->getStatement(0);
            auto idName = std::dynamic_pointer_cast<syntax_tree::Identifier>(id)->getValue();

            if (idName == name) {
                return false; // имя затенено
            }
        }
    }

    // 3. Аналогично для lambda
    if (node->getNodeType() == "λ") {
        auto pattern = node->getStatement(0);
        if (isRecursive(name, pattern)) {
            return false;
        }
    }

    // 4. Рекурсивный обход
    for (auto& child : node->getStatements()) {
        if (isRecursive(name, child)) {
            return true;
        }
    }

    return false;
}

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

void Translator::translateString(std::shared_ptr<syntax_tree::ASTNode> node, int index, std::shared_ptr<syntax_tree::ASTNode> parent) {
    if (auto n = std::dynamic_pointer_cast<syntax_tree::LiteralString>(node)) {
        std::string value = n->getValue();
        
        std::shared_ptr<syntax_tree::ASTNode> arg;
        if (value == "") {
            arg = std::make_shared<syntax_tree::Constructor>("CONSTRUCTOR");
            arg->addStatement(std::make_shared<syntax_tree::ASTNode>("EMPTY"));
        }
        else {
            arg = std::make_shared<syntax_tree::Operator>(":");
            for (int i = 0; i < value.size(); i++) {
                auto c = std::make_shared<syntax_tree::Constructor>("CONSTRUCTOR");
                c->addStatement(std::make_shared<syntax_tree::ASTNode>("Char"));
                auto ascii = std::make_shared<syntax_tree::LiteralInt>("LiteralInt", cBigNumber(std::to_string(value[i]).c_str(), 10));
                c->addStatement(ascii);
                arg->addStatement(c);
            }
            translateList(arg);
        }
        
        auto s = std::make_shared<syntax_tree::Constructor>("CONSTRUCTOR");
        s->addStatement(std::make_shared<syntax_tree::ASTNode>("String"));
        s->addStatement(arg);
        parent->getStatement(index) = s;
    }
}

void Translator::translateChar(std::shared_ptr<syntax_tree::ASTNode> node, int index, std::shared_ptr<syntax_tree::ASTNode> parent) {
    if (auto n = std::dynamic_pointer_cast<syntax_tree::LiteralChar>(node)) {
        char value = n->getValue();
        auto c = std::make_shared<syntax_tree::Constructor>("CONSTRUCTOR");
        c->addStatement(std::make_shared<syntax_tree::ASTNode>("Char"));
        auto ascii = std::make_shared<syntax_tree::LiteralInt>("LiteralInt", cBigNumber(std::to_string(value).c_str(), 10));
        c->addStatement(ascii);

        parent->getStatement(index) = c;
    }
}