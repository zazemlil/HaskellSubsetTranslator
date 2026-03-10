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
    void generate(std::shared_ptr<syntax_tree::ASTNode> node, 
                    std::unordered_map<std::string, std::vector<std::shared_ptr<syntax_tree::ASTNode>>> groups);

private:
    std::shared_ptr<syntax_tree::ASTNode> buildFunction(const std::string& name,
                                                                        const std::vector<std::shared_ptr<syntax_tree::ASTNode>>& decls);

    size_t getArity(std::vector<std::shared_ptr<syntax_tree::ASTNode>> decls);
    std::vector<std::shared_ptr<syntax_tree::ASTNode>> generateParams(size_t arity);
    std::shared_ptr<syntax_tree::ASTNode> buildCase(std::vector<std::shared_ptr<syntax_tree::ASTNode>> params,
                                                        const std::vector<std::shared_ptr<syntax_tree::ASTNode>>& decls);
    std::shared_ptr<syntax_tree::ASTNode> compileMatch(std::vector<std::shared_ptr<syntax_tree::ASTNode>> vars, 
                                                                    std::vector<Clause> clauses);
    std::string getPatternTag(std::shared_ptr<syntax_tree::ASTNode> p);

    std::shared_ptr<syntax_tree::ASTNode> extractSignature(std::vector<std::shared_ptr<syntax_tree::ASTNode>> decl);
    std::vector<Clause> buildClauses(const std::vector<std::shared_ptr<syntax_tree::ASTNode>>& decls);

};
