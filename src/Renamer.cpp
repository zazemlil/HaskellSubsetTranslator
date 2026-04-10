#include "Renamer.h"

using namespace syntax_tree;

void Renamer::rename(std::shared_ptr<ASTNode> root) {
    std::unordered_map<std::string, std::string> env;
    renameNode(root, env);
}

std::string Renamer::freshName(const std::string& base) {
    int& cnt = counters[base]; // если нет - создастся 0
    std::string name = base + "_" + std::to_string(cnt);
    cnt++;
    return name;
}

void Renamer::renameNode(std::shared_ptr<ASTNode> node, std::unordered_map<std::string, std::string>& env) {
    if (!node) return;

    const std::string type = node->getNodeType();

    if (type == "Identifier") {
        auto id = std::dynamic_pointer_cast<Identifier>(node);
        auto it = env.find(id->getValue());

        if (it != env.end()) {
            id->setValue(it->second);
        }
        return;
    }

    if (type == "λ") {
        auto newEnv = env;

        renamePattern(node->getStatement(0), newEnv);
        renameNode(node->getStatement(1), newEnv);
        return;
    }

    if (type == "LET" || type == "LC_LET") {
        auto newEnv = env;

        std::shared_ptr<ASTNode> defs;
        if (type == "LET") {
            defs = node->getStatement(0);
        }
        else if (type == "LC_LET") {
            defs = node;
        }

        // идентификаторы объявлений
        std::unordered_map<std::string, std::string> tmpEnv;
        for (auto& d : defs->getStatements()) {
            if (d->getNodeType() == "SIGNATURE") continue;

            auto nameNode = d->getStatement(0);
            auto id = std::dynamic_pointer_cast<Identifier>(nameNode);

            if (tmpEnv.find(id->getValue()) == tmpEnv.end()) {
                std::string newName = freshName(id->getValue());
                newEnv[id->getValue()] = newName;
                tmpEnv[id->getValue()] = newName;
            }

            id->setValue(newEnv[id->getValue()]);
        }

        // сигнатуры 
        for (auto& d : defs->getStatements()) {
            if (d->getNodeType() == "SIGNATURE") {
                auto nameNode = d->getStatement(0);

                auto id = std::dynamic_pointer_cast<Identifier>(nameNode);

                if (newEnv.count(id->getValue())) {
                    id->setValue(newEnv[id->getValue()]);
                }
            }
        }

        // объявления
        for (auto& d : defs->getStatements()) {
            renameNode(d, newEnv);
        }

        if (type == "LET") {
            // тело let
            renameNode(node->getStatement(1), newEnv);
        }
        return;
    }

    if (type == "WHERE") {
        auto newEnv = env;

        auto defs = node->getStatement(1);

        // идентификаторы объявлений
        std::unordered_map<std::string, std::string> tmpEnv;
        for (auto& d : defs->getStatements()) {
            if (d->getNodeType() == "SIGNATURE") continue;

            auto nameNode = d->getStatement(0);
            auto id = std::dynamic_pointer_cast<Identifier>(nameNode);

            if (tmpEnv.find(id->getValue()) == tmpEnv.end()) {
                std::string newName = freshName(id->getValue());
                newEnv[id->getValue()] = newName;
                tmpEnv[id->getValue()] = newName;
            }

            id->setValue(newEnv[id->getValue()]);
        }

        // сигнатуры
        for (auto& d : defs->getStatements()) {
            if (d->getNodeType() == "SIGNATURE") {
                auto nameNode = d->getStatement(0);
                auto id = std::dynamic_pointer_cast<Identifier>(nameNode);

                if (newEnv.count(id->getValue())) {
                    id->setValue(newEnv[id->getValue()]);
                }
            }
        }

        // тело where
        renameNode(node->getStatement(0), newEnv);

        // объявления
        for (auto& d : defs->getStatements()) {
            renameNode(d, newEnv);
        }
        return;
    }

    if (type == "CASE") {
        renameNode(node->getStatement(0), env);

        auto alts = node->getStatement(1);
        for (auto& alt : alts->getStatements()) {
            auto newEnv = env;

            renamePattern(alt->getStatement(0), newEnv);
            renameNode(alt->getStatement(1), newEnv);
        }
        return;
    }

    if (type == "DEF" || type == "=") {
        auto newEnv = env;

        auto nameNode = node->getStatement(0);

        // параметры (PATTERNS)
        auto patterns = nameNode->getStatements();

        for (auto& p : patterns) {
            renamePattern(p, newEnv);
        }

        renameNode(node->getStatement(1), newEnv);
        return;
    }

    if (type == "LIST_COMPREHENSION") {
        auto newEnv = env;

        auto expr = node->getStatement(0);
        auto qualifiers = node->getStatement(1);

        for (auto& q : qualifiers->getStatements()) {
            auto qType = q->getNodeType();

            if (qType == "LC_LET") { // need to fix
                // auto id = std::dynamic_pointer_cast<Identifier>(q->getStatement(0)->getStatement(0));

                // std::string newName = freshName(id->getValue());
                // newEnv[id->getValue()] = newName;
                // id->setValue(newName);
                
                // renameNode(q->getStatement(0)->getStatement(0), newEnv);
                renameNode(q, newEnv);
            }
            else if (qType == "<-") {
                renameNode(q->getStatement(1), newEnv);
                renamePattern(q->getStatement(0), newEnv);
            }
            else { // FILTER (expr)
                renameNode(q, newEnv);
            }
        }

        renameNode(expr, newEnv);
        return;
    }

    for (auto& child : node->getStatements()) {
        renameNode(child, env);
    }
}

void Renamer::renamePattern(std::shared_ptr<ASTNode> pattern, std::unordered_map<std::string, std::string>& env) {
    if (!pattern) return;

    const std::string type = pattern->getNodeType();

    // x
    if (type == "Identifier") {
        auto id = std::dynamic_pointer_cast<Identifier>(pattern);

        std::string newName = freshName(id->getValue());

        env[id->getValue()] = newName;
        id->setValue(newName);
        return;
    }

    // _
    if (type == "_")
        return;

    // (Just x) or (x, y)
    if (type == "CONSTRUCTOR_PATTERN" || type == "TUPLE_PATTERN") {
        for (auto& p : pattern->getStatements()) {
            renamePattern(p, env);
        }
        return;
    }

    // список [x,y]
    if (type == "LIST" || type == "LIST_PATTERN") {
        for (auto& p : pattern->getStatements()) {
            renamePattern(p, env);
        }
        return;
    }

    // (x:xs)
    if (type == "LIST_HEAD_TAIL_PATTERN") {
        renamePattern(pattern->getStatement(0), env);
        renamePattern(pattern->getStatement(1), env);
        return;
    }

    // литералы — ничего не делаем
}
