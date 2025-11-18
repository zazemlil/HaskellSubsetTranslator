%{
#include "Parser.hpp"
#include "AST.h"

using namespace haskell_subset;
void set_input_file(yyscan_t scanner, const char* filename);

static int current_line = 1;
static int current_column = 1;

%}

%option reentrant batch noyywrap yylineno nodefault outfile="Scanner.cpp" header="Scanner.hpp"

%{
#define YY_USER_ACTION \
    do { \
        if (yylloc) { \
            yylloc->begin.line = yylloc->end.line = current_line; \
            yylloc->begin.column = current_column; \
            yylloc->end.column = current_column + yyleng - 1; \
        } \
        current_column += yyleng; \
    } while (0);
%}

%%

"-" { return Parser::token::T_ARITHMETIC_OP_MINUS; }
"+" { return Parser::token::T_ARITHMETIC_OP_PLUS; }
"*" { return Parser::token::T_ARITHMETIC_OP_MULTIPLY; }
"/" { return Parser::token::T_ARITHMETIC_OP_DIVIDE; }

"||" { return Parser::token::T_LOGIC_OP_OR; }
"&&" { return Parser::token::T_LOGIC_OP_AND; }
"==" { return Parser::token::T_LOGIC_OP_EQUAL; }
"<=" { return Parser::token::T_LOGIC_OP_LESS_OR_EQUAL; }
">=" { return Parser::token::T_LOGIC_OP_MORE_OR_EQUAL; }
"/=" { return Parser::token::T_LOGIC_OP_NOT_EQUAL; }
"<" { return Parser::token::T_LOGIC_OP_LESS; }
">" { return Parser::token::T_LOGIC_OP_MORE; }

"=" { return Parser::token::T_ASSIGNMENT; }

"Int"  { return Parser::token::T_TYPE_INT; }
"Float"  { return Parser::token::T_TYPE_FLOAT; }
"Char"  { return Parser::token::T_TYPE_CHAR; }
"Bool"  { return Parser::token::T_TYPE_BOOLEAN; }


"if"  { return Parser::token::T_IF; }
"then"  { return Parser::token::T_THEN; }
"else"  { return Parser::token::T_ELSE; }
"let"  { return Parser::token::T_LET; }
"in"  { return Parser::token::T_IN; }
"data"  { return Parser::token::T_DATA; }

\\ { return Parser::token::T_LAMBDA; }
"->" { return Parser::token::T_ARROW_RIGHT; }
"<-" { return Parser::token::T_ARROW_LEFT; }
"_" { return Parser::token::T_UNDERSCORE; }
"|" { return Parser::token::T_DEVIDING_LINE; }
"," { return Parser::token::T_COMMA; }
"::" { return Parser::token::T_COLON_DOUBLE; }
":" { return Parser::token::T_COLON; }
"'" { return Parser::token::T_SINGLE_QUOTE; }
"(" { return Parser::token::T_PARENTHESIS_OPEN; }
")" { return Parser::token::T_PARENTHESIS_CLOSE; }
"[" { return Parser::token::T_BRACKET_OPEN; }
"]" { return Parser::token::T_BRACKET_CLOSE; }

"True"              { yylval->emplace<std::string>(std::string(yytext, yyleng)); return Parser::token::T_LITERAL_BOOLEAN; }
"False"             { yylval->emplace<std::string>(std::string(yytext, yyleng)); return Parser::token::T_LITERAL_BOOLEAN; }
[a-z][a-zA-z|0-9]*  { yylval->emplace<std::string>(std::string(yytext, yyleng)); return Parser::token::T_IDENTIFIER; }
[A-Z][a-zA-z|0-9]*  { yylval->emplace<std::string>(std::string(yytext, yyleng)); return Parser::token::T_TYPE_CONSTRUCTOR; }
[-]?[0-9]+          { yylval->emplace<std::string>(std::string(yytext, yyleng)); return Parser::token::T_LITERAL_INT; }
[-]?[0-9]+"."[0-9]+ { yylval->emplace<float>(atof(yytext)); return Parser::token::T_LITERAL_FLOAT; }
\"[^"]*\"           { yylval->emplace<std::string>(std::string(yytext + 1, yyleng - 2)); return Parser::token::T_LITERAL_STRING; }

\n     { current_line++; current_column = 1; }
\r     { current_column = 1; }
[ \t]+ {}

<<EOF>> { return Parser::token::T_END_OF_FILE; }
. { return yytext[0]; }

%%

void set_input_file(yyscan_t scanner, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file) {
        yyset_in(file, scanner);
    } else {
        std::cerr << "Cannot open file: " << filename << std::endl;
        exit(1);
    }
}

syntax_tree::AST analize(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return syntax_tree::AST();
    }
    
    yyscan_t scanner;
    yylex_init(&scanner);
    
    set_input_file(scanner, argv[1]);
    
    syntax_tree::AST result;
    haskell_subset::Parser parser{ scanner, result };
    if (parser.parse() == 0) {
        std::cout << "Parse success.\n";
    }
    else {
        std::cerr << "Parse error!\n";
    }
    
    yylex_destroy(scanner);

    return result;
}