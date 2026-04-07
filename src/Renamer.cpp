#include "Renamer.h"

using namespace syntax_tree;

// =======================
// PUBLIC
// =======================

void Renamer::rename(std::shared_ptr<ASTNode> root)
{
    std::unordered_map<std::string, std::string> env;
    renameNode(root, env);
}

// =======================
// UTILS
// =======================

std::string Renamer::freshName(const std::string& base)
{
    return base + "_" + std::to_string(counter++);
}

// =======================
// CORE
// =======================

void Renamer::renameNode(
    std::shared_ptr<ASTNode> node,
    std::unordered_map<std::string, std::string>& env)
{
    if (!node) return;

    const std::string type = node->getNodeType();

    // =======================
    // IDENTIFIER (usage)
    // =======================
    if (type == "IDENTIFIER")
    {
        auto id = std::dynamic_pointer_cast<Identifier>(node);
        auto it = env.find(id->getValue());

        if (it != env.end())
        {
            id->setValue(it->second);
        }
        return;
    }

    // =======================
    // LAMBDA
    // λ x -> body
    // =======================
    if (type == "λ")
    {
        auto newEnv = env;

        auto param = node->getStatement(0);
        auto id = std::dynamic_pointer_cast<Identifier>(param);

        std::string newName = freshName(id->getValue());

        newEnv[id->getValue()] = newName;
        id->setValue(newName);

        renameNode(node->getStatement(1), newEnv);
        return;
    }

    // =======================
    // LET
    // =======================
    if (type == "LET")
    {
        auto newEnv = env;

        auto defs = node->getStatement(0);

        // 1. сначала объявляем имена
        for (auto& d : defs->getStatements())
        {
            if (d->getNodeType() == "SIGNATURE") continue;

            auto nameNode = d->getStatement(0);
            auto id = std::dynamic_pointer_cast<Identifier>(nameNode);

            std::string newName = freshName(id->getValue());

            newEnv[id->getValue()] = newName;
            id->setValue(newName);
        }

        // 2. сигнатуры
        for (auto& d : defs->getStatements())
        {
            if (d->getNodeType() == "SIGNATURE")
            {
                auto nameNode = d->getStatement(0);
                auto id = std::dynamic_pointer_cast<Identifier>(nameNode);

                if (newEnv.count(id->getValue()))
                {
                    id->setValue(newEnv[id->getValue()]);
                }
            }
        }

        // 3. обрабатываем тела
        for (auto& d : defs->getStatements())
        {
            renameNode(d, newEnv);
        }

        // 4. тело let
        renameNode(node->getStatement(1), newEnv);
        return;
    }

    // =======================
    // WHERE
    // =======================
    if (type == "WHERE")
    {
        auto newEnv = env;

        auto defs = node->getStatement(1);

        // объявление
        for (auto& d : defs->getStatements())
        {
            if (d->getNodeType() == "SIGNATURE") continue;

            auto nameNode = d->getStatement(0);
            auto id = std::dynamic_pointer_cast<Identifier>(nameNode);

            std::string newName = freshName(id->getValue());

            newEnv[id->getValue()] = newName;
            id->setValue(newName);
        }

        // сигнатуры
        for (auto& d : defs->getStatements())
        {
            if (d->getNodeType() == "SIGNATURE")
            {
                auto nameNode = d->getStatement(0);
                auto id = std::dynamic_pointer_cast<Identifier>(nameNode);

                if (newEnv.count(id->getValue()))
                {
                    id->setValue(newEnv[id->getValue()]);
                }
            }
        }

        // тело выражения
        renameNode(node->getStatement(0), newEnv);

        // определения
        for (auto& d : defs->getStatements())
        {
            renameNode(d, newEnv);
        }

        return;
    }

    // =======================
    // CASE
    // =======================
    if (type == "CASE")
    {
        // выражение
        renameNode(node->getStatement(0), env);

        auto alts = node->getStatement(1);

        for (auto& alt : alts->getStatements())
        {
            auto newEnv = env;

            auto pattern = alt->getStatement(0);

            renamePattern(pattern, newEnv);

            renameNode(alt->getStatement(1), newEnv);
        }

        return;
    }

    // =======================
    // DEFINITION
    // =======================
    if (type == "DEF" || type == "=")
    {
        auto newEnv = env;

        auto nameNode = node->getStatement(0);

        // параметры (PATTERNS)
        if (nameNode->getStatementCount() > 0)
        {
            auto patterns = nameNode->getStatement(0);

            for (auto& p : patterns->getStatements())
            {
                renamePattern(p, newEnv);
            }
        }

        renameNode(node->getStatement(1), newEnv);
        return;
    }

    // =======================
    // DEFAULT: рекурсивно
    // =======================
    for (auto& child : node->getStatements())
    {
        renameNode(child, env);
    }
}

// =======================
// PATTERNS
// =======================

void Renamer::renamePattern(
    std::shared_ptr<ASTNode> pattern,
    std::unordered_map<std::string, std::string>& env)
{
    if (!pattern) return;

    const std::string type = pattern->getNodeType();

    // x
    if (type == "IDENTIFIER")
    {
        auto id = std::dynamic_pointer_cast<Identifier>(pattern);

        std::string newName = freshName(id->getValue());

        env[id->getValue()] = newName;
        id->setValue(newName);
        return;
    }

    // _
    if (type == "WILDCARD")
        return;

    // конструктор (Just x)
    if (type == "CONSTRUCTOR" || type == "TUPLE_PATTERN")
    {
        for (auto& p : pattern->getStatements())
        {
            renamePattern(p, env);
        }
        return;
    }

    // список [x,y]
    if (type == "LIST")
    {
        for (auto& p : pattern->getStatements())
        {
            renamePattern(p, env);
        }
        return;
    }

    // (x:xs)
    if (type == ":")
    {
        renamePattern(pattern->getStatement(0), env);
        renamePattern(pattern->getStatement(1), env);
        return;
    }

    // литералы — ничего не делаем
}
