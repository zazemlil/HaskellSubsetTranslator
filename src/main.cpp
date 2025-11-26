#include <iostream>
#include "AST.h"

extern syntax_tree::AST analize(int argc, char* argv[]);

int main(int argc, char* argv[])
{
    syntax_tree::AST ast = analize(argc, argv);
    ast.print();
    
    return 0;
}