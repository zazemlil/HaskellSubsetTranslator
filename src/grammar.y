%{
#include "Scanner.hpp"
%}

%require "3.7.4"
%language "C++"
%defines "Parser.hpp"
%output "Parser.cpp"

%define api.parser.class {Parser}
%define api.namespace {haskell_subset}
%define api.value.type variant
%param {yyscan_t scanner}
%parse-param {syntax_tree::AST& result}

%locations

%code requires {
    #include "AST.h"
}

%code provides
{
    #define YY_DECL \
        int yylex(haskell_subset::Parser::semantic_type *yylval, \
        haskell_subset::Parser::location_type* yylloc, \
        yyscan_t yyscanner)
    YY_DECL;
}

%type <std::unique_ptr<syntax_tree::ASTNode>> expr

%token <std::string> T_IDENTIFIER
%token <int> T_LITERAL_INT
%token <float> T_LITERAL_FLOAT 
%token <char> T_LITERAL_CHAR 
%token T_LITERAL_BOOLEAN_TRUE T_LITERAL_BOOLEAN_FALSE

%token T_TYPE_INT T_TYPE_FLOAT T_TYPE_CHAR T_TYPE_BOOLEAN T_TYPE_LIST

%token T_ARITHMETIC_OP_PLUS T_ARITHMETIC_OP_MINUS T_ARITHMETIC_OP_MULTIPLY T_ARITHMETIC_OP_DIVIDE
%token T_LOGIC_OP_OR T_LOGIC_OP_AND T_LOGIC_OP_EQUAL T_LOGIC_OP_NOT_EQUAL T_LOGIC_OP_MORE T_LOGIC_OP_LESS T_LOGIC_OP_MORE_OR_EQUAL T_LOGIC_OP_LESS_OR_EQUAL

%token T_ASSIGNMENT

%token T_IF T_THEN T_ELSE
%token T_LET T_IN
%token T_DO
%token T_DATA
%token <std::string> T_TYPE_CONSTRUCTOR

%token T_LAMBDA
%token T_ARROW_RIGHT T_ARROW_LEFT
%token T_UNDERSCORE
%token T_DEVIDING_LINE
%token T_COMMA
%token T_COLON_DOUBLE
%token T_COLON
%token T_SINGLE_QUOTE
%token T_PARENTHESIS_OPEN T_PARENTHESIS_CLOSE
%token T_BRACKET_OPEN T_BRACKET_CLOSE

%token T_END_OF_FILE

%%

program: expr T_LITERAL_INT T_IF T_THEN T_ELSE T_IDENTIFIER T_END_OF_FILE {
    result = syntax_tree::AST(std::move($1));
    YYACCEPT;
};

expr: %empty {
    $$ = std::make_unique<syntax_tree::ASTNode>("testNode");
};

%%

void haskell_subset::Parser::error(const location_type& loc, const std::string& msg) {
    const char* text = yyget_text(scanner);
    int length = yyget_leng(scanner);
    
    std::cerr << msg << " at (Line: " << loc.begin.line << ", Column: " << loc.begin.column
            << ", Last token: '" << std::string(text, length) << "')\n";
}