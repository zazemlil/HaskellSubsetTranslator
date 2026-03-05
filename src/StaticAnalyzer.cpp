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

void StaticAnalyzer::checkPatternRedundancy(const std::vector<std::shared_ptr<syntax_tree::ASTNode>> &decls) {
    std::cout << "StaticAnalyzer::checkPatternRedundancy\n";
}