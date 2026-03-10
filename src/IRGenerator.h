#pragma once
#include "AST.h"

#include <unordered_map>

struct Clause
{
    std::vector<std::shared_ptr<syntax_tree::ASTNode>> patterns;
    std::shared_ptr<syntax_tree::ASTNode> body;
};


class IRGenerator {
public:
    std::shared_ptr<syntax_tree::ASTNode> generate(std::shared_ptr<syntax_tree::ASTNode> node, 
                                                    std::unordered_map<std::string, std::vector<std::shared_ptr<syntax_tree::ASTNode>>>);

//private:
    std::shared_ptr<syntax_tree::ASTNode> buildFunction(const std::string& name,
                                                                        const std::vector<std::shared_ptr<syntax_tree::ASTNode>>& decls);

    size_t getArity(std::vector<std::shared_ptr<syntax_tree::ASTNode>> decls);
    std::vector<std::shared_ptr<syntax_tree::ASTNode>> generateParams(size_t arity);

    std::shared_ptr<syntax_tree::ASTNode> extractSignature(std::vector<std::shared_ptr<syntax_tree::ASTNode>> decl);
    std::vector<Clause> buildClauses(const std::vector<std::shared_ptr<syntax_tree::ASTNode>>& decls);

};
