#include "StaticAnalyzer.h"


void StaticAnalyzer::analyze(std::shared_ptr<syntax_tree::ASTNode> root) {
    if (!root) return;
    analyzeNode(root);
}

void StaticAnalyzer::analyzeNode(std::shared_ptr<syntax_tree::ASTNode> node) {
    if (!node) return;

    if (node->getNodeType() == "DEFINITIONS") {
        auto& decls = node->getStatements();

        auto groups = groupByName(decls);

        checkContiguity(decls);
        for (auto& [name, gdecls] : groups)
        {
            checkArity(gdecls);
            checkPatternRedundancy(gdecls);
        }
    }

    if (node->getNodeType() == "LET") {
        auto& decls = node->getStatement(0)->getStatements();

        auto groups = groupByName(decls);

        checkContiguity(decls);
        for (auto& [name, gdecls] : groups)
        {
            checkArity(gdecls);
            checkPatternRedundancy(gdecls);
        }
    }

    for (auto& child : node->getStatements()) {
        analyzeNode(child);
    }
}

std::unordered_map<std::string, std::vector<std::shared_ptr<syntax_tree::ASTNode>>> 
    StaticAnalyzer::groupByName(const std::vector<std::shared_ptr<syntax_tree::ASTNode>> &decls) 
{
    std::unordered_map<std::string, std::vector<std::shared_ptr<syntax_tree::ASTNode>>> groups;

    for (const auto& decl : decls)
    {
        auto nameNode = decl->getStatement(0);
        auto id = std::dynamic_pointer_cast<syntax_tree::Identifier>(nameNode);
        groups[id->getValue()].push_back(decl);
    }

    return groups;
}

void StaticAnalyzer::checkContiguity(const std::vector<std::shared_ptr<syntax_tree::ASTNode>> &decls) {
    std::unordered_map<std::string, int> lastPos;

    for (size_t i = 0; i < decls.size(); ++i) {
        if (decls[i]->getNodeType() == "SIGNATURE") continue;

        auto nameNode = decls[i]->getStatement(0);
        auto id = std::dynamic_pointer_cast<syntax_tree::Identifier>(nameNode);

        if (!id) continue;

        std::string name = id->getValue();

        if (lastPos.count(name) && lastPos[name] != i - 1) {
            throw std::runtime_error(
                "Non-contiguous definitions for function '" + name + "'"
            );
        }

        lastPos[name] = i;
    }
}

void StaticAnalyzer::checkArity(const std::vector<std::shared_ptr<syntax_tree::ASTNode>> &decls) {
    if (decls.empty())
        return;

    auto first = decls[0];

    size_t expectedArity = 0;

    if (first->getNodeType() == "SIGNATURE") {
        expectedArity = first->getStatement(1)->getStatementCount()-1;
    } else if (first->getNodeType() == "DEF" || first->getNodeType() == "=") {
        auto id = first->getStatement(0);
        if (id->getStatementCount() > 0) {
            if (id->getStatement(0)->getNodeType() == "PATTERNS") {
                expectedArity = id->getStatement(0)->getStatementCount();
            }
        }
    }

    for (const auto& decl : decls)
    {
        size_t arity = 0;

        if (decl->getNodeType() == "SIGNATURE") {
            arity = decl->getStatement(1)->getStatementCount()-1;
        } else if (decl->getNodeType() == "DEF" || decl->getNodeType() == "=") {
            auto id = decl->getStatement(0);
            if (id->getStatementCount() > 0) {
                if (id->getStatement(0)->getNodeType() == "PATTERNS") {
                    arity = id->getStatement(0)->getStatementCount();
                }
            }
        }

        if (arity != expectedArity)
        {
            auto nameNode = decl->getStatement(0);
            auto id = std::dynamic_pointer_cast<syntax_tree::Identifier>(nameNode);
            throw std::runtime_error(
                "Arity mismatch in function '" + id->getValue() + "'"
            );
        }
    }

    if (expectedArity == 0 && decls.size() > 1) {
        auto nameNode = decls[0]->getStatement(0);
        auto id = std::dynamic_pointer_cast<syntax_tree::Identifier>(nameNode);
        throw std::runtime_error(
            "Multiple definitions of '" + id->getValue() + "'"
        );
    }
}

std::vector<std::shared_ptr<syntax_tree::ASTNode>> 
    StaticAnalyzer::extractPatterns(std::shared_ptr<syntax_tree::ASTNode> decl) 
{
    auto& stmts = decl->getStatements();

    std::vector<std::shared_ptr<syntax_tree::ASTNode>> patterns;

    for (size_t i = 1; i < stmts.size()-1; i++)
        patterns.push_back(stmts[i]);

    return patterns;
}

bool StaticAnalyzer::rowCovers(
    const std::vector<std::shared_ptr<syntax_tree::ASTNode>>& r1,
    const std::vector<std::shared_ptr<syntax_tree::ASTNode>>& r2)
{
    if (r1.size() != r2.size())
        return false;

    for (size_t i = 0; i < r1.size(); i++) {

        if (!patternCovers(r1[i], r2[i]))
            return false;
    }

    return true;
}

bool StaticAnalyzer::patternCovers(std::shared_ptr<syntax_tree::ASTNode> p1, std::shared_ptr<syntax_tree::ASTNode> p2) {
    auto k1 = getPatternKind(p1);
    auto k2 = getPatternKind(p2);

    if (k1 == WILDCARD || k1 == IDENTIFIER)
        return true;

    if (k1 == LITERAL && k2 == LITERAL) {

        std::ostringstream a,b;
        p1->printValue(a);
        p2->printValue(b);

        return a.str() == b.str();
    }

    if (k1 == CONSTRUCTOR && k2 == CONSTRUCTOR) {

        std::ostringstream a,b;
        p1->printValue(a);
        p2->printValue(b);

        if (a.str() != b.str())
            return false;

        auto args1 = p1->getStatements();
        auto args2 = p2->getStatements();

        if (args1.size() != args2.size())
            return false;

        for (size_t i=0;i<args1.size();i++)
            if (!patternCovers(args1[i], args2[i]))
                return false;

        return true;
    }

    if (k1 == LIST_EMPTY && k2 == LIST_EMPTY)
        return true;

    if (k1 == LIST_CONS && k2 == LIST_CONS)
        return true;

    if (k1 == LIST_ENUM && k2 == LIST_ENUM) {

        auto a = p1->getStatements();
        auto b = p2->getStatements();

        if (a.size() != b.size())
            return false;

        for (size_t i=0;i<a.size();i++)
            if (!patternCovers(a[i], b[i]))
                return false;

        return true;
    }

    return false;
}

PatternKind StaticAnalyzer::getPatternKind(std::shared_ptr<syntax_tree::ASTNode> p) {
    std::string t = p->getNodeType();

    if (t == "Identifier") {
        auto id = std::dynamic_pointer_cast<syntax_tree::Identifier>(p);
        if (id->getValue() == "_")
            return WILDCARD;
        return IDENTIFIER;
    }

    if (t == "LiteralInt" ||
        t == "LiteralFloat" ||
        t == "LiteralString" ||
        t == "LiteralBoolean")
        return LITERAL;

    if (t == "LiteralTypeConstructor")
        return CONSTRUCTOR;

    if (t == "LiteralNil")
        return LIST_EMPTY;

    if (t == "ListNode")
        return LIST_ENUM;

    return IDENTIFIER;
}

void StaticAnalyzer::checkPatternRedundancy(const std::vector<std::shared_ptr<syntax_tree::ASTNode>> &decls) {
    std::vector<std::vector<std::shared_ptr<syntax_tree::ASTNode>>> seenRows;

    for (auto& d : decls) {

        auto row = extractPatterns(d);

        bool redundant = false;

        for (auto& prev : seenRows) {

            if (rowCovers(prev, row)) {
                redundant = true;
                break;
            }
        }

        if (redundant) {

            throw std::runtime_error(
                "Redundant pattern in function definition");
        }

        seenRows.push_back(row);
    }
}