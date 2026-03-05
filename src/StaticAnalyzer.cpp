#include "StaticAnalyzer.h"

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
    std::cout << "StaticAnalyzer::checkContiguity\n";
}

void StaticAnalyzer::checkArity(const std::vector<std::shared_ptr<syntax_tree::ASTNode>> &decls) {
    std::cout << "StaticAnalyzer::checkArity\n";
}

void StaticAnalyzer::checkPatternRedundancy(const std::vector<std::shared_ptr<syntax_tree::ASTNode>> &decls) {
    std::cout << "StaticAnalyzer::checkPatternRedundancy\n";
}