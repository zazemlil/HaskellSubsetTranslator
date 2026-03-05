#include <iostream>
#include "AST.h"
#include "StaticAnalyzer.h"

extern syntax_tree::AST analize(int argc, char* argv[]);

int main(int argc, char* argv[])
{
    syntax_tree::AST ast = analize(argc, argv);
    ast.print();

    StaticAnalyzer* staticAnalyzer = new StaticAnalyzer();
    try {
        staticAnalyzer->analyze(ast.getRoot());
    }
    catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
    }

    return 0;
}