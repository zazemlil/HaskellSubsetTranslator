#pragma once
#include "AST.h"

#include <unordered_map>

class IRGenerator {
public:
    std::shared_ptr<syntax_tree::ASTNode> generate(std::shared_ptr<syntax_tree::ASTNode> node, 
                                                    std::unordered_map<std::string, std::vector<std::shared_ptr<syntax_tree::ASTNode>>>);

//private:
    std::shared_ptr<syntax_tree::ASTNode> buildFunction(const std::string& name,
                                                                        const std::vector<std::shared_ptr<syntax_tree::ASTNode>>& decls);

    size_t getArity(std::vector<std::shared_ptr<syntax_tree::ASTNode>> decls);
    std::vector<std::shared_ptr<syntax_tree::ASTNode>> generateParams(size_t arity);
};
