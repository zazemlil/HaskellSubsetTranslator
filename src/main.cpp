#include <iostream>
#include "AST.h"
#include "StaticAnalyzer.h"

extern std::tuple<syntax_tree::AST, syntax_tree::AST> analyze(int argc, char* argv[]);

int main(int argc, char* argv[])
{
    auto [ast, dataDeclarations] = analyze(argc, argv);
    if (ast.isEmpty()) {
        return -1;
    }
    std::cout << "---------------------- AST ----------------------\n";
    ast.print();
    std::cout << "--------------- Data Declarations ---------------\n";
    dataDeclarations.print();

    StaticAnalyzer* staticAnalyzer = new StaticAnalyzer();
    syntax_tree::AST ir;
    try {
        staticAnalyzer->analyze(ast.getRoot());
        // after successfull static analyze: ast == ir
        ir = ast;
    }
    catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
        return -2;
    }

    
    std::cout << "---------------------- IR -----------------------\n";
    ir.print();

    std::cout << "Success.\n";
    return 0;
}