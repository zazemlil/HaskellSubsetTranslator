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

%token <std::string> T_IDENTIFIER
%token <std::string> T_LITERAL_INT
%token <float> T_LITERAL_FLOAT 
%token <std::string> T_LITERAL_STRING
%token <bool> T_LITERAL_BOOLEAN

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

%type <std::shared_ptr<syntax_tree::ASTNode>> s expr literal  
%type <std::shared_ptr<syntax_tree::ASTNode>> infix_expr or_expr and_expr and_expr_tail
%type <std::shared_ptr<syntax_tree::ASTNode>> comp_expr comp_expr_tail comp_op
%type <std::shared_ptr<syntax_tree::ASTNode>> additive_expr additive_expr_tail additive_op
%type <std::shared_ptr<syntax_tree::ASTNode>> multiplicative_expr multiplicative_expr_tail multiplicative_op
%type <std::shared_ptr<syntax_tree::ASTNode>> term list_elements list_elements_tail
%type <std::shared_ptr<syntax_tree::ASTNode>> function_call arg_list

%%
// ===============================================================

s: expr T_END_OF_FILE {
    // just for example
    std::shared_ptr<syntax_tree::ASTNode> node = std::make_shared<syntax_tree::ASTNode>("testNode");
    result = syntax_tree::AST(std::move(node));
    YYACCEPT;
};

// ============= ПРОДУКЦИИ С ИНФИКСНЫМИ ОПЕРАТОРАМИ (9) ==========

infix_expr: and_expr or_expr {};
or_expr: T_LOGIC_OP_OR and_expr or_expr {}
    | %empty {};

and_expr: comp_expr and_expr_tail {};
and_expr_tail: T_LOGIC_OP_AND comp_expr and_expr_tail {}
    | %empty {};

comp_expr: additive_expr comp_expr_tail {};
comp_expr_tail: comp_op additive_expr comp_expr_tail {}
    | %empty {};
comp_op: T_LOGIC_OP_EQUAL {}
    | T_LOGIC_OP_NOT_EQUAL {}
    | T_LOGIC_OP_LESS {}
    | T_LOGIC_OP_MORE {}
    | T_LOGIC_OP_LESS_OR_EQUAL {}
    | T_LOGIC_OP_MORE_OR_EQUAL {};

additive_expr: multiplicative_expr additive_expr_tail {};
additive_expr_tail: additive_op multiplicative_expr additive_expr_tail {}
    | %empty {};
additive_op: T_ARITHMETIC_OP_PLUS {}
    | T_ARITHMETIC_OP_MINUS {};

multiplicative_expr: term multiplicative_expr_tail {};
multiplicative_expr_tail: multiplicative_op term multiplicative_expr_tail {}
    | %empty {};
multiplicative_op: T_ARITHMETIC_OP_MULTIPLY {}
    | T_ARITHMETIC_OP_DIVIDE {};

term: literal {}
    | T_IDENTIFIER {}
    | T_TYPE_CONSTRUCTOR {}
    | T_PARENTHESIS_OPEN expr T_PARENTHESIS_CLOSE {}
    | T_BRACKET_OPEN list_elements T_BRACKET_CLOSE {}
    | T_PARENTHESIS_OPEN function_call T_PARENTHESIS_CLOSE {};

list_elements: expr list_elements_tail {}
    | %empty {};
list_elements_tail: T_COMMA expr list_elements_tail {}
    | %empty {};

function_call: T_IDENTIFIER term arg_list {};
arg_list: term arg_list {}
    | %empty {};

// ==================== БАЗОВЫЕ ПРОДУКЦИИ (1) ====================

expr: infix_expr {};

literal: T_LITERAL_INT { $$ = std::make_shared<syntax_tree::LiteralInt>("LiteralInt", cBigNumber($1.c_str(), 10)); }
    | T_LITERAL_FLOAT { $$ = std::make_shared<syntax_tree::LiteralFloat>("LiteralFloat", $1); }
    | T_LITERAL_STRING { $$ = std::make_shared<syntax_tree::LiteralString>("LiteralString", $1); }
    | T_LITERAL_BOOLEAN { $$ = std::make_shared<syntax_tree::LiteralBoolean>("LiteralBool", $1); };

// ===============================================================
%%

void haskell_subset::Parser::error(const location_type& loc, const std::string& msg) {
    const char* text = yyget_text(scanner);
    int length = yyget_leng(scanner);
    
    std::cerr << msg << " at (Line: " << loc.begin.line << ", Column: " << loc.begin.column
            << ", Last token: '" << std::string(text, length) << "')\n";
}