#include "StaticAnalyzer.h"
#include <unordered_map>

void StaticAnalyzer::analyze(std::shared_ptr<syntax_tree::ASTNode> root) {
    if (!root) return;
    analyzeNode(root);
}

void StaticAnalyzer::analyzeNode(std::shared_ptr<syntax_tree::ASTNode> node) {
    if (!node) return;

    if (node->getNodeType() == "DEFINITIONS") {
        auto& decls = node->getStatements();

        checkContiguity(decls);
        checkArity(decls);
        checkPatternRedundancy(decls);
    }

    if (node->getNodeType() == "LET") {
        auto& decls = node->getStatements();

        checkContiguity(decls);
        checkArity(decls);
        checkPatternRedundancy(decls);
    }

    for (auto& child : node->getStatements()) {
        analyzeNode(child);
    }
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
                "Non-contiguous definitions for function " + name
            );
        }

        lastPos[name] = i;
    }
}

void StaticAnalyzer::checkArity(const std::vector<std::shared_ptr<syntax_tree::ASTNode>> &decls) {
    std::cout << "StaticAnalyzer::checkArity\n";
}

void StaticAnalyzer::checkPatternRedundancy(const std::vector<std::shared_ptr<syntax_tree::ASTNode>> &decls) {
    std::cout << "StaticAnalyzer::checkPatternRedundancy\n";
}