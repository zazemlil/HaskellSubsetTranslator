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

%type <std::shared_ptr<syntax_tree::ASTNode>> s expr literal id literal_int literal_float literal_string literal_boolean type_constructor
%type <std::shared_ptr<syntax_tree::ASTNode>> or_expr and_expr comp_expr additive_expr multiplicative_expr
%type <std::shared_ptr<syntax_tree::ASTNode>> term list_elements list_elements_tail
%type <std::shared_ptr<syntax_tree::ASTNode>> function_call arg_list

%type <std::shared_ptr<syntax_tree::ASTNode>> let_expr bindings bind
%type <std::shared_ptr<syntax_tree::ASTNode>> if_expr lambda_expr

%%
// ===============================================================

s: expr T_END_OF_FILE { 
    result = syntax_tree::AST($1);
    YYACCEPT;
};

// ============= Let, if, lambda (2) ==========

if_expr: T_IF expr T_THEN expr T_ELSE expr {
    auto n = std::make_shared<syntax_tree::ASTNode>("COND");
    n->addStatement($2);
    n->addStatement($4);
    n->addStatement($6);
    $$ = n;
};

// ============= ПРОДУКЦИИ С ИНФИКСНЫМИ ОПЕРАТОРАМИ (9) ==========

or_expr:
    or_expr T_LOGIC_OP_OR and_expr {
        auto n = std::make_shared<syntax_tree::ASTNode>("||");
        n->addStatement($1);
        n->addStatement($3);
        $$ = n;
    }
    | and_expr { $$ = $1; };

and_expr:
    and_expr T_LOGIC_OP_AND comp_expr {
        auto n = std::make_shared<syntax_tree::ASTNode>("&&");
        n->addStatement($1);
        n->addStatement($3);
        $$ = n;
    }
    | comp_expr { $$ = $1; };

comp_expr:
    comp_expr T_LOGIC_OP_EQUAL additive_expr {
        auto n = std::make_shared<syntax_tree::ASTNode>("==");
        n->addStatement($1);
        n->addStatement($3);
        $$ = n;
    }
    | comp_expr T_LOGIC_OP_NOT_EQUAL additive_expr {
        auto n = std::make_shared<syntax_tree::ASTNode>("/=");
        n->addStatement($1);
        n->addStatement($3);
        $$ = n;
    }
    | comp_expr T_LOGIC_OP_LESS additive_expr {
        auto n = std::make_shared<syntax_tree::ASTNode>("<");
        n->addStatement($1);
        n->addStatement($3);
        $$ = n;
    }
    | comp_expr T_LOGIC_OP_MORE additive_expr {
        auto n = std::make_shared<syntax_tree::ASTNode>(">");
        n->addStatement($1);
        n->addStatement($3);
        $$ = n;
    }
    | comp_expr T_LOGIC_OP_LESS_OR_EQUAL additive_expr {
        auto n = std::make_shared<syntax_tree::ASTNode>("<=");
        n->addStatement($1);
        n->addStatement($3);
        $$ = n;
    }
    | comp_expr T_LOGIC_OP_MORE_OR_EQUAL additive_expr {
        auto n = std::make_shared<syntax_tree::ASTNode>(">=");
        n->addStatement($1);
        n->addStatement($3);
        $$ = n;
    }
    | additive_expr { $$ = $1; };

additive_expr:
    additive_expr T_ARITHMETIC_OP_PLUS multiplicative_expr {
        auto n = std::make_shared<syntax_tree::ASTNode>("+");
        n->addStatement($1);
        n->addStatement($3);
        $$ = n;
    }
    | additive_expr T_ARITHMETIC_OP_MINUS multiplicative_expr {
        auto n = std::make_shared<syntax_tree::ASTNode>("-");
        n->addStatement($1);
        n->addStatement($3);
        $$ = n;
    }
    | multiplicative_expr { $$ = $1; };

multiplicative_expr:
    multiplicative_expr T_ARITHMETIC_OP_MULTIPLY term {
        auto n = std::make_shared<syntax_tree::ASTNode>("*");
        n->addStatement($1);
        n->addStatement($3);
        $$ = n;
    }
    | multiplicative_expr T_ARITHMETIC_OP_DIVIDE term {
        auto n = std::make_shared<syntax_tree::ASTNode>("/");
        n->addStatement($1);
        n->addStatement($3);
        $$ = n;
    }
    | term { $$ = $1; };

term: literal { $$ = $1; }
    | id { $$ = $1; }
    | type_constructor { $$ = $1; }
    | T_PARENTHESIS_OPEN expr T_PARENTHESIS_CLOSE { $$ = $2; }
    | T_BRACKET_OPEN list_elements T_BRACKET_CLOSE { $$ = $2; }
    | T_PARENTHESIS_OPEN function_call T_PARENTHESIS_CLOSE { $$ = $2; };

list_elements: expr list_elements_tail {
        auto l = std::make_shared<syntax_tree::ListNode>("LIST");
        l->addStatement($1);
        l->addStatements($2->getStatements());
        $$ = l;
    }
    | %empty { $$ = std::make_shared<syntax_tree::LiteralNil>("NIL"); };
list_elements_tail: T_COMMA expr list_elements_tail {
        auto l = std::make_shared<syntax_tree::ListNode>("LIST");
        l->addStatement($2);
        l->addStatements($3->getStatements());
        $$ = l;
    }
    | %empty { $$ = std::make_shared<syntax_tree::LiteralNil>("NIL"); };

function_call: id term arg_list {
    auto n = std::make_shared<syntax_tree::ASTNode>("CALL");
    n->addStatement($1);
    auto args = std::make_shared<syntax_tree::ListNode>("LIST");
    args->addStatement($2);
    args->addStatements($3->getStatements());
    n->addStatement(args);
    $$ = n;
};
arg_list: term arg_list {
        auto l = std::make_shared<syntax_tree::ListNode>("LIST");
        l->addStatement($1);
        l->addStatements($2->getStatements());
        $$ = l;
    }
    | %empty { $$ = std::make_shared<syntax_tree::LiteralNil>("NIL"); };

// ==================== БАЗОВЫЕ ПРОДУКЦИИ (1) ====================

expr: or_expr { $$ = $1; }
    | if_expr { $$ = $1; };

literal: literal_int { $$ = $1; }
    | literal_float { $$ = $1; }
    | literal_string { $$ = $1; }
    | literal_boolean { $$ = $1; };

id: T_IDENTIFIER { $$ = std::make_shared<syntax_tree::Identifier>("Identifier", $1); };
type_constructor: T_TYPE_CONSTRUCTOR { $$ = std::make_shared<syntax_tree::LiteralTypeConstructor>("LiteralTypeConstructor", $1); };
literal_int: T_LITERAL_INT { $$ = std::make_shared<syntax_tree::LiteralInt>("LiteralInt", cBigNumber($1.c_str(), 10)); };
literal_float: T_LITERAL_FLOAT { $$ = std::make_shared<syntax_tree::LiteralFloat>("LiteralFloat", $1); };
literal_string: T_LITERAL_STRING { $$ = std::make_shared<syntax_tree::LiteralString>("LiteralString", $1); };
literal_boolean: T_LITERAL_BOOLEAN { $$ = std::make_shared<syntax_tree::LiteralBoolean>("LiteralBool", $1); };

// ===============================================================
%%

void haskell_subset::Parser::error(const location_type& loc, const std::string& msg) {
    const char* text = yyget_text(scanner);
    int length = yyget_leng(scanner);
    
    std::cerr << msg << " at (Line: " << loc.begin.line << ", Column: " << loc.begin.column
            << ", Last token: '" << std::string(text, length) << "')\n";
}