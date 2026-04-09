#pragma once

#include "AST.h"

class Renamer {
public:
    void rename(std::shared_ptr<syntax_tree::ASTNode> root);

private:
    std::unordered_map<std::string, int> counters;

    std::string freshName(const std::string& base);

    void renameNode(
        std::shared_ptr<syntax_tree::ASTNode> node,
        std::unordered_map<std::string, std::string>& env
    );

    void renamePattern(
        std::shared_ptr<syntax_tree::ASTNode> pattern,
        std::unordered_map<std::string, std::string>& env
    );
};
