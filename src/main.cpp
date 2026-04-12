#include "AST.h"
#include "Renamer.h"
#include "ScopeChecker.h"
#include "StaticAnalyzer.h"
#include "Translator.h"

extern std::tuple<syntax_tree::AST, syntax_tree::AST> analyze(char* arg, bool printError);

int main(int argc, char* argv[]) {
    bool s_flag = false, a_flag = false, t_flag = false, ir_flag = false;
    int new_argc = argc;
    for (int i = 1; i < new_argc; i++) {
        if (std::string(argv[i]) == "-s") {
            s_flag = true;
            for (int j = i; j < new_argc - 1; j++) { argv[j] = argv[j + 1]; }
            new_argc--; i--;
        }
        if (std::string(argv[i]) == "-a") {
            a_flag = true;
            for (int j = i; j < new_argc - 1; j++) { argv[j] = argv[j + 1]; }
            new_argc--; i--;
        }
        if (std::string(argv[i]) == "+t") {
            t_flag = true;
            for (int j = i; j < new_argc - 1; j++) { argv[j] = argv[j + 1]; }
            new_argc--; i--;
        }
        if (std::string(argv[i]) == "+ir") {
            ir_flag = true;
            for (int j = i; j < new_argc - 1; j++) { argv[j] = argv[j + 1]; }
            new_argc--; i--;
        }
    }
    argc = new_argc;

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " [params] <input_file> [<output_file>]" << std::endl;
        return 1;
    }

    auto [ast, dataDeclarations] = analyze(argv[1], !s_flag);
    if (ast.isEmpty()) {
        std::cerr << "Error: Input file is empty.\n";
        return 2;
    }

    if (t_flag) {
        std::cout << "-------------------------------------------------\n";
        std::cout << "---------------------- AST ----------------------\n";
        std::cout << "-------------------------------------------------\n";
        ast.print();
        std::cout << "-------------------------------------------------\n";
        std::cout << "--------------- Data Declarations ---------------\n";
        std::cout << "-------------------------------------------------\n";
        dataDeclarations.print();
    }

    Renamer* renamer = new Renamer();
    renamer->rename(ast.getRoot());
    delete renamer;

    if (t_flag) {
        std::cout << "-------------------------------------------------\n";
        std::cout << "----------------- AST (renamed) -----------------\n";
        std::cout << "-------------------------------------------------\n";
        ast.print();
    }

    ScopeChecker* scopeChecker = new ScopeChecker();
    try {
        scopeChecker->analyze(ast.getRoot());
        delete scopeChecker;
    } catch(const std::exception& e) {
        if (!a_flag) {
            std::cerr << "Scope Error: " << e.what() << '\n';
        }
        delete scopeChecker;
        return 3;
    }

    StaticAnalyzer* staticAnalyzer = new StaticAnalyzer();
    syntax_tree::AST ir;
    std::shared_ptr<syntax_tree::ASTNode> signatures;
    try {
        staticAnalyzer->analyze(ast.getRoot());
        // after successfull static analyze: ast == ir
        ir = ast;
        signatures = staticAnalyzer->getIRGenerator()->getSignatures();
        delete staticAnalyzer;
    }
    catch(const std::exception& e) {
        if (!a_flag) {
            std::cerr << "Error: " << e.what() << '\n';
        } 
        delete staticAnalyzer;
        return 4;
    }

    if (ir_flag) {
        std::cout << "-------------------------------------------------\n";
        std::cout << "---------------------- IR -----------------------\n";
        std::cout << "-------------------------------------------------\n";
        signatures->print();
        ir.print();
    }

    Translator* translator = new Translator();
    syntax_tree::AST dataDeclsELC = translator->translate(dataDeclarations);
    syntax_tree::AST extendedLambdaCalculus = translator->translate(ir);
    delete translator;

    if (argc < 3) {
        std::cout << "-------------------------------------------------\n";
        std::cout << "------------ Extended lambda calculus -----------\n";
        std::cout << "-------------------------------------------------\n";
        for (auto& n : dataDeclsELC.getRoot()->getStatements()) {
            n->printFlat();
            std::cout << "\n";
        }
        std::cout << "\n";
        for (auto& n : signatures->getStatements()) {
            n->printFlat();
            std::cout << "\n";
        }
        std::cout << "\n";
        for (auto& n : extendedLambdaCalculus.getRoot()->getStatements()) {
            n->printFlat();
            std::cout << "\n";
        }
    } 
    else {
        auto fileStream = std::make_unique<std::ofstream>(argv[2], std::ios::out | std::ios::trunc);
        if (fileStream->is_open()) {
            for (auto& n : dataDeclsELC.getRoot()->getStatements()) {
                n->printFlat(0, *fileStream);
                *fileStream << "\n";
            }
            *fileStream << "\n";
            for (auto& n : signatures->getStatements()) {
                n->printFlat(0, *fileStream);
                *fileStream << "\n";
            }
            *fileStream << "\n";
            for (auto& n : extendedLambdaCalculus.getRoot()->getStatements()) {
                n->printFlat(0, *fileStream);
                *fileStream << "\n";
            }
            fileStream->close();
        }
        else {
            std::cerr << "Error: Output file is not open.\n";
        }
    }

    std::cout << "\nSuccess.\n";
    return 0;
}