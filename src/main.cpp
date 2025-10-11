#include <iostream>
#include "AST.h"

extern syntax_tree::AST analize(int argc, char* argv[]);

int main(int argc, char* argv[])
{
    // auto root = std::make_unique<ASTNode>("Program");
    // auto func1 = std::make_unique<ASTNode>("Function");
    // auto func2 = std::make_unique<ASTNode>("Function");
    // auto body1 = std::make_unique<ASTNode>("Body1");
    // auto body2 = std::make_unique<ASTNode>("Body2");
    
    // body1->addStatement(std::make_unique<ASTNode>("Return1"));
    // body1->addStatement(std::make_unique<ASTNode>("Return2"));
    // func1->addStatement(std::move(body2));
    // func2->addStatement(std::move(body1));
    // root->addStatement(std::move(func1));
    // root->addStatement(std::move(func2));

    // AST ast(std::move(root));
    // ast.print();
    
    syntax_tree::AST ast = analize(argc, argv);
    ast.print();
    
    return 0;
}