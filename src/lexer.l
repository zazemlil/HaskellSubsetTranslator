%{
#include "Parser.hpp"
#include "AST.h"
//#include <stack>
//#include <queue>

using namespace haskell_subset;
void set_input_file(yyscan_t scanner, const char* filename);

static int current_line = 1;
static int current_column = 1;

// static std::stack<int> indentStack;
// static std::queue<int> pendingTokens;

// void pushToken(int tok) {
//     pendingTokens.push(tok);
// }

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

#define YY_DECL \
    int yylex_core(haskell_subset::Parser::semantic_type *yylval, \
                   haskell_subset::Parser::location_type* yylloc, \
                   yyscan_t yyscanner)
%}

%%

--.* { /* ignore comment */ }

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
"String"  { return Parser::token::T_TYPE_STRING; }
"Bool"  { return Parser::token::T_TYPE_BOOLEAN; }


"if"  { return Parser::token::T_IF; }
"then"  { return Parser::token::T_THEN; }
"else"  { return Parser::token::T_ELSE; }
"let"  {
    //indentStack.push(current_column-3);
    //pushToken(Parser::token::T_CURLY_BRACKET_OPEN); 
    return Parser::token::T_LET; 
}
"where" { return Parser::token::T_WHERE;  }
"in"  { return Parser::token::T_IN; }
"case"  { return Parser::token::T_CASE; }
"of"  { return Parser::token::T_OF; }
"data"  { return Parser::token::T_DATA; }
"otherwise"  { return Parser::token::T_OTHERWISE; }

\\ { return Parser::token::T_LAMBDA; }
"->" { return Parser::token::T_ARROW_RIGHT; }
"<-" { return Parser::token::T_ARROW_LEFT; }
"_" { return Parser::token::T_UNDERSCORE; }
"|" { return Parser::token::T_DEVIDING_LINE; }
"," { return Parser::token::T_COMMA; }
"::" { return Parser::token::T_COLON_DOUBLE; }
":" { return Parser::token::T_COLON; }
";" { return Parser::token::T_SEMICOLON; }
"'" { return Parser::token::T_SINGLE_QUOTE; }
"(" { return Parser::token::T_PARENTHESIS_OPEN; }
")" { return Parser::token::T_PARENTHESIS_CLOSE; }
"[" { return Parser::token::T_BRACKET_OPEN; }
"]" { return Parser::token::T_BRACKET_CLOSE; }
"{" { return Parser::token::T_CURLY_BRACKET_OPEN; }
"}" { return Parser::token::T_CURLY_BRACKET_CLOSE; }

"True"              { yylval->emplace<bool>(true); return Parser::token::T_LITERAL_BOOLEAN; }
"False"             { yylval->emplace<bool>(false); return Parser::token::T_LITERAL_BOOLEAN; }
[a-z][a-zA-Z0-9]*  { yylval->emplace<std::string>(std::string(yytext, yyleng)); return Parser::token::T_IDENTIFIER; }
[A-Z][a-zA-Z0-9]*  { yylval->emplace<std::string>(std::string(yytext, yyleng)); return Parser::token::T_TYPE_CONSTRUCTOR; }
[-]?[0-9]+          { yylval->emplace<std::string>(std::string(yytext, yyleng)); return Parser::token::T_LITERAL_INT; }
[-]?[0-9]+"."[0-9]+ { yylval->emplace<float>(atof(yytext)); return Parser::token::T_LITERAL_FLOAT; }
\"[^"]*\"           { yylval->emplace<std::string>(std::string(yytext + 1, yyleng - 2)); return Parser::token::T_LITERAL_STRING; }

\n[ \t]*     { 
    current_line++; current_column = yyleng; 

    // int indent = yyleng;

    // if (!indentStack.empty()) {
    //     int top = indentStack.top();
    //     std::cout << "indent = " << indent << ", top = " << top << "\n";
    //     if (indent == top) {
    //         pushToken(Parser::token::T_SEMICOLON);
    //     } else if (indent < top) {
    //         while (!indentStack.empty() && indent < indentStack.top()) {
    //             indentStack.pop();
    //             pushToken(Parser::token::T_CURLY_BRACKET_CLOSE);
    //         }
    //     }
    //     std::cout << "pendingTokens(front) = " << pendingTokens.front() << "\n";
    // }
}
\r     { current_column = 1; }
[ \t]+ {}

<<EOF>> { return Parser::token::T_END_OF_FILE; }
. { return yytext[0]; }

%%

int yylex(haskell_subset::Parser::semantic_type *yylval,
          haskell_subset::Parser::location_type* yylloc,
          yyscan_t yyscanner) 
{
    // if (!pendingTokens.empty()) {
    //     int tok = pendingTokens.front();
    //     std::cout << tok << "\n"; //
    //     pendingTokens.pop();
    //     return tok;
    // }
    int r = yylex_core(yylval, yylloc, yyscanner);
    //std::cout << r << "\n";
    return r;
}

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