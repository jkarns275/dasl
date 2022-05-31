%skeleton "lalr1.cc" /* -*- C++ -*- */
%require "3.0"
%defines
%define api.parser.class { Parser }
%define api.token.constructor
%define api.value.type variant
%define parse.assert
%define api.namespace { dasl }
%code requires
{
    #include <stdint.h>
    
    #include <iostream>
    #include <string>
    #include <vector>
    #include <memory>
    using namespace std;

    #include <location.hh>

    #include <parse_tree.hxx>
    using namespace dasl;
    using namespace dasl::pt;

    namespace dasl {
        class Lexer;
    }
}

// Bison calls yylex() function that must be provided by us to suck tokens
// from the lexer. This block will be placed at the beginning of IMPLEMENTATION file (cpp).
// We define this function here (function! not method).
// This function is called only inside Bison, so we make it static to limit symbol visibility for the linker
// to avoid potential linking conflicts.
%code top
{
    #include <iostream>
    #include <lexer.hxx>
    #include <parser.hxx>
    using namespace dasl;
    using namespace dasl::pt;
//    #include "interpreter.h"
    
    // yylex() arguments are defined in parser.y
    static dasl::Parser::symbol_type yylex(dasl::Lexer &lexer) {
        return lexer.get_next_token();
    }
    
    // you can accomplish the same thing by inlining the code using preprocessor
    // x and y are same as in above static function
    // #define yylex(x, y) lexer.get_next_token()
}

%lex-param { dasl::Lexer &lexer }
// %lex-param { dasl::Interpreter &driver }
%parse-param { dasl::Lexer &lexer }
%parse-param { dasl::pt::Env &env }
%locations
%define parse.trace
%define parse.error verbose

%define api.token.prefix {TOKEN_}

%token END 0 "end of file"

// values
%token <std::string> STRING  "string";
%token <std::string> ATOM    "atom"
%token <uint64_t> INT "int";
%token <double> FLOAT "float";
%token <std::string> ID "id";


// Symbols
%token CBOPEN     "cbopen"
%token CBCLOSE    "cbclose"
%token POPEN      "(";
%token PCLOSE     ")";
%token SEMICOLON  ";";
%token COMMA      ",";
%token TAIL       "::";
%token COLON      ":";
%token SQ_BOPEN   "["
%token SQ_BCLOSE  "]"
%token DOT        "."
%token ADD        "+"
%token SUB        "-"
%token MUL        "*"
%token DIV        "/"
%token MOD        "%"
%token RSH        ">>"
%token LSH        "<<"
%token LAND       "&&"
%token LOR        "||"
%token LXOR       "^^"
%token ARROW      "arrow"
%token EQ         "=="
%token NEQ        "!="
%token GTE        ">="
%token GT         ">"
%token LTE        "<="
%token LT         "<"
%token BAND       "band"
%token BOR        "bor"
%token BXOR       "bxor"
%token NOT        "!"
%token INV        "~"
%token ASSIGN     "="

// Keywords
%token KW_DO      "do";
%token KW_END     "end";
%token KW_DEF     "def";
%token KW_MODULE  "module";
%token KW_IF      "if";
%token KW_THEN    "then";
%token KW_ELSE    "else";
%token KW_FOR     "for";
%token KW_IN      "in";
%token KW_CASE    "case";
%token KW_OF      "of";
%token KW_TYPE    "type";
%token KW_INT     "kw_int";
%token KW_FLOAT   "kw_float";
%token KW_BOOL    "bool";
%token KW_TRUE    "true";
%token KW_FALSE   "false";
%token KW_ATOM    "kw_atom";
%token KW_ANY     "kw_any";
%token KW_LIST    "kw_list";
%token KW_STRING  "kw_string";
%token KW_MAP     "kw_map";
%token KW_VAL     "val";
%token EOF        "eof";

%type< int > program;
%type< vector<unique_ptr<St>> > stmt_list;
%type< SymbolRef > symbol;
%type< unique_ptr<Type> > type;
%type< unique_ptr<Pat> > pat;
%type< unique_ptr<Pat> > typed_pat;
%type< unique_ptr<Pat> > list_pat;
%type< unique_ptr<Pat> > value_pat;
%type< unique_ptr<Pat> > map_pat;
%type< unique_ptr<Pat> > record_pat;
%type< unique_ptr<Pat> > symbol_pat;
%type< vector<unique_ptr<Pat>> > pat_list;
%type< MapPatEntry > map_pat_entry;
%type< vector<MapPatEntry> > map_pat_entry_list;
%type< RecordPatField > record_pat_field;
%type< vector<RecordPatField> > record_pat_field_list;
%type< Id > id;
%type< unique_ptr<Expr> > primary_expr;
%type< unique_ptr<Expr> > postfix_expr;
%type< unique_ptr<Expr> > unary_expr;
%type< unique_ptr<Expr> > mult_expr;
%type< unique_ptr<Expr> > add_expr;
%type< unique_ptr<Expr> > shift_expr;
%type< unique_ptr<Expr> > comp_expr;
%type< unique_ptr<Expr> > eq_expr;
%type< unique_ptr<Expr> > band_expr;
%type< unique_ptr<Expr> > bxor_expr;
%type< unique_ptr<Expr> > bor_expr;
%type< unique_ptr<Expr> > land_expr;
%type< unique_ptr<Expr> > lxor_expr;
%type< unique_ptr<Expr> > lor_expr;
// %type< vector<unique_ptr<Expr>> > expr_inner;
%type< unique_ptr<Expr> > expr;
%type< unique_ptr<Expr> > if_expr;
%type< unique_ptr<Expr> > list_expr;
%type< vector<unique_ptr<Expr>> > expr_list;
%type< vector<unique_ptr<Expr>> > call_args;
%type< RecordExprField > record_expr_field;
%type< vector<RecordExprField> > record_expr_field_list;
%type< unique_ptr<Expr> > record_expr;
%type< unique_ptr<St> > expr_stmt;
%type< unique_ptr<St> > stmt;
%type< unique_ptr<St> > def_stmt;
%type< unique_ptr<St> > val_stmt;
%type< unique_ptr<St> > record_stmt;
%type< unique_ptr<St> > module_stmt;
%type< vector<unique_ptr<St>> > body;
%type< vector<unique_ptr<St>> > module_body;
%type< vector<RecordEntry> > record_entry_list;
%type< RecordEntry > record_entry;
%type< vector<unique_ptr<Pat>> > def_arg_list;
%type< vector<unique_ptr<Pat>> > def_args;
%type< AtomValue > atom;

%type< Case > case_;
%type< vector<Case> > cases;
%type< unique_ptr<Expr> > case_expr;

%type< vector<unique_ptr<Expr>> > argument_expr_list;
%type< vector<unique_ptr<Expr>> > compound_expr;
%type< UnOpExpr::UnOp > un_op;
%type< BinOpExpr::BinOp > mult_op;
%type< BinOpExpr::BinOp > add_op;
%type< BinOpExpr::BinOp > shift_op;
%type< BinOpExpr::BinOp > comp_op;
%type< BinOpExpr::BinOp > eq_op;

%start program

%%

id 
  : ID { $$ = Id(env.interner.get($1)); }
  ;

atom 
  : ATOM { string x {$1, 1}; $$ = AtomValue(env.interner.get(x)); }
  ;

symbol 
  : id            { $$ = SymbolRef($1); }
  | symbol DOT id { $1.shift($3); $$ = move($1); }
  ;

type 
  : KW_LIST { $$ = make_unique<ListType>(); }
  | KW_MAP  { $$ = make_unique<MapType>();  }
  | symbol { $$ = make_unique<RecordType>($1); }
  | KW_ANY { $$ = make_unique<AnyType>(); }
  | KW_STRING { $$ = make_unique<PrimType>(PrimType::STRING); }
  | KW_INT { $$ = make_unique<PrimType>(PrimType::INT); }
  | KW_FLOAT { $$ = make_unique<PrimType>(PrimType::FLOAT); }
  | KW_BOOL { $$ = make_unique<PrimType>(PrimType::BOOL); }
  | KW_ATOM { $$ = make_unique<PrimType>(PrimType::ATOM); }
  | POPEN PCLOSE { $$ = make_unique<PrimType>(PrimType::UNIT); }
  ;

pat_list 
  : typed_pat { vector<unique_ptr<Pat>> v; v.push_back(move($1)); $$ = move(v); }
  | pat_list COMMA typed_pat { $1.push_back(move($3)); $$ = move($1); }
  ;

list_pat 
  : "[" "]" { $$ = make_unique<ListPat>(); } 
  | "[" pat_list "]" { $$ = ListPat::make($2); }
  | "[" pat_list "::" typed_pat "]" { $$ = ListPat::make($2, move($4)); }
  ;

expr_list
  : expr { vector<unique_ptr<Expr>> e; e.push_back(move($1)); $$ = move(e); }
  | expr_list COMMA expr { $1.push_back(move($3)); $$ = move($1); }
  ;

list_expr
  : "[" "]" { $$ = make_unique<ListExpr>(); }
  | "[" expr_list "]" { $$ = ListExpr::make($2); }
  | "[" expr_list "::" expr "]" { $$ = ListExpr::make($2, move($4)); }
  ;

map_pat_entry 
  : pat COLON pat { $$ = make_pair(move($1), move($3)); }
  | pat ARROW pat { $$ = make_pair(move($1), move($3)); }
  ;

map_pat_entry_list 
  : map_pat_entry { vector<pair<unique_ptr<Pat>, unique_ptr<Pat>>> x; x.push_back(move($1)); $$ = move(x); }
  | map_pat_entry_list COMMA map_pat_entry { $1.push_back(move($3)); $$ = move($1); }
  ;

map_pat 
  : CBOPEN CBCLOSE { $$ = make_unique<MapPat>(); }
  | CBOPEN map_pat_entry_list CBCLOSE { $$ = make_unique<MapPat>($2); }
  ;

record_pat_field 
  : id COLON pat { $$ = make_pair(AtomValue($1.val), move($3)); }
  ;

record_pat_field_list
  : record_pat_field { vector<pair<AtomValue, unique_ptr<Pat>>> x; x.push_back(move($1)); $$ = move(x); }
  | record_pat_field_list COMMA record_pat_field { $1.push_back(move($3)); $$ = move($1); }
  ;

record_pat
  : symbol CBOPEN CBCLOSE { $$ = make_unique<RecordPat>($1); }
  | symbol CBOPEN record_pat_field_list CBCLOSE { $$ = make_unique<RecordPat>($1, $3); }
  ;

symbol_pat 
  : symbol { $$ = make_unique<SymbolPat>($1); }
  ;

value_pat
  : INT { $$ = make_unique<ValuePat>(Value($1)); }
  | "(" ")" { $$ = make_unique<ValuePat>(Value(Unit())); }
  | STRING { $$ = make_unique<ValuePat>(Value(StringValue(env.interner.get($1)))); }
  | FLOAT { $$ = make_unique<ValuePat>(Value($1)); }
  | "true" { $$ = make_unique<ValuePat>(Value(true)); }
  | "false" { $$ = make_unique<ValuePat>(Value(false)); }
  | atom { $$ = make_unique<ValuePat>(Value($1)); }
  ;

pat
  : list_pat { $$ = move($1); }
  | map_pat { $$ = move($1); }
  | record_pat { $$ = move($1); }
  | symbol_pat { $$ = move($1); }
  | value_pat { $$ = move($1); }
  ;

typed_pat 
  : pat COLON type { $1->set_type($3); $$ = move($1); }
  | pat { $$ = move($1); }
  ;

def_arg_list
  : def_arg_list "," typed_pat { $1.push_back(move($3)); $$ = move($1); }
  | typed_pat { vector<unique_ptr<Pat>> v; v.push_back(move($1)); $$ = move(v); }
  ;

def_args
  : "(" ")" { vector<unique_ptr<Pat>> v; $$ = move(v); }
  | "(" def_arg_list ")" { $$ = move($2); }
  ;

body
  // : body stmt { $1.push_back(move($2)); $$ = move($1); }
  : body ";" stmt { $1.push_back(move($3)); $$ = move($1); }
  | stmt { vector<unique_ptr<St>> x; x.push_back(move($1)); $$ = move(x); }
  ;

def_stmt
  : "def" id def_args "do" body "end" { $$ = make_unique<DefSt>($2, $3, $5); }
  | "def" id def_args "arrow" type "do" body "end" { $$ = make_unique<DefSt>($2, $3, $5, $7); }
  ;

record_entry
  : atom ARROW type { $$ = make_pair($1, move($3)); }
  | id COLON type { $$ = make_pair(AtomValue($1.val), move($3)); }
  ;

record_entry_list
  : record_entry_list COMMA record_entry { $1.push_back(move($3)); $$ = move($1); }
  | record_entry { vector<RecordEntry> v; v.push_back(move($1)); $$ = move(v); }
  ;

record_stmt
  : "type" id ASSIGN CBOPEN CBCLOSE { $$ = make_unique<RecordSt>($2); }
  | "type" id ASSIGN CBOPEN record_entry_list CBCLOSE { $$ = make_unique<RecordSt>($2, $5); }
  ;

val_stmt
  : "val" id "=" expr { $$ = make_unique<ValSt>($2, $4); }
  ;

module_body
  : module_body stmt { $1.push_back(move($2)); $$ = move($1); }
  | module_body ";" stmt { $1.push_back(move($3)); $$ = move($1); }
  | stmt { vector<unique_ptr<St>> v; v.push_back(move($1)); $$ = move(v); }
  ;

module_stmt
  : "module" id module_body "end" { $$ = make_unique<ModuleSt>($2, $3); }
  ;

expr_stmt
  : expr { $$ = make_unique<ExprSt>($1); }
  ;

stmt
  : module_stmt { $$ = move($1); }
  | val_stmt { $$ = move($1); }
  | record_stmt { $$ = move($1); }
  | def_stmt { $$ = move($1); }
  // | expr_stmt { $$ = move($1); }
  ;

record_expr_field
  : id ":" expr { $$ = make_pair(AtomValue($1), move($3)); }
  | atom ARROW expr { $$ = make_pair($1, move($3)); }
  ;

record_expr_field_list
  : record_expr_field_list "," record_expr_field { $1.push_back(move($3)); $$ = move($1); }
  | record_expr_field { vector<pair<AtomValue, unique_ptr<Expr>>> v; v.push_back(move($1)); $$ = move(v); }
  ;

record_expr
  : symbol CBOPEN record_expr_field_list CBCLOSE { $$ = make_unique<RecordExpr>($1, $3); }
  | symbol CBOPEN CBCLOSE { $$ = make_unique<RecordExpr>($1); }
  ;

// compound_expr
//   : compound_expr ";" expr { $1.push_back(move($3)); $$ = move($1); }
//   | expr { vector<unique_ptr<Expr>> e; e.push_back(move($1)); $$ = move(e); }
//   ;

primary_expr 
  : STRING { $$ = make_unique<ValueExpr>(Value(StringValue(env.interner.get($1)))); }
  | atom { $$ = make_unique<ValueExpr>(Value($1)); }
  | record_expr { $$ = move($1); }
  | symbol { $$ = make_unique<SymbolExpr>($1); }
  | INT { $$ = make_unique<ValueExpr>(Value($1)); }
  | FLOAT { $$ = make_unique<ValueExpr>(Value($1)); }
  | POPEN PCLOSE { $$ = make_unique<ValueExpr>(Value(Unit())); }
  | KW_FALSE { $$ = make_unique<ValueExpr>(Value(false)); }
  | KW_TRUE { $$ = make_unique<ValueExpr>(Value(true)); }
  | POPEN expr PCLOSE { $$ = move($2); }
  | list_expr { $$ = move($1); }
  ;

postfix_expr
  : primary_expr { $$ = move($1); }
  | postfix_expr "[" expr "]" { $$ = make_unique<BinOpExpr>($1, $3, BinOpExpr::INDEX); }
  | symbol call_args { $$ = make_unique<CallExpr>($1, $2); }
  ;

call_args
  : "(" ")" { $$ = vector<unique_ptr<Expr>>(); }
  | "(" argument_expr_list ")" { $$ = move($2); }
  ;

argument_expr_list
  : expr { vector<unique_ptr<Expr>> args; args.push_back(move($1)); $$ = move(args); }
  | argument_expr_list COMMA expr { $1.push_back(move($3)); $$ = move($1); }
  ;

un_op
  : NOT { $$ = UnOpExpr::NOT; }
  | SUB { $$ = UnOpExpr::NEG; }
  | INV { $$ = UnOpExpr::INV; }
  ;

unary_expr
  : un_op postfix_expr { $$ = make_unique<UnOpExpr>($2, $1); }
  | postfix_expr { $$ = move($1); }
  ;

mult_op
  : MUL { $$ = BinOpExpr::MUL; }
  | DIV { $$ = BinOpExpr::DIV; }
  | MOD { $$ = BinOpExpr::MOD; }
  ;

mult_expr
  : mult_expr mult_op unary_expr { $$ = make_unique<BinOpExpr>($1, $3, $2); }
  | unary_expr { $$ = move($1); }
  ;

add_op
  : ADD { $$ = BinOpExpr::ADD; }
  | SUB { $$ = BinOpExpr::SUB; }
  ;

add_expr
  : add_expr add_op mult_expr { $$ = make_unique<BinOpExpr>($1, $3, $2); }
  | mult_expr { $$ = move($1); }
  ;

shift_op
  : LSH { $$ = BinOpExpr::LSH; }
  | RSH { $$ = BinOpExpr::RSH; }
  ;

shift_expr
  : shift_expr shift_op add_expr { $$ = make_unique<BinOpExpr>($1, $3, $2); }
  | add_expr { $$ = move($1); }
  ;

comp_op
  : GT { $$ = BinOpExpr::GT; }
  | GTE { $$ = BinOpExpr::GTE; }
  | LT { $$ = BinOpExpr::LT; }
  | LTE { $$ = BinOpExpr::LTE; }
  ;

comp_expr
  : comp_expr comp_op shift_expr { $$ = make_unique<BinOpExpr>($1, $3, $2); }
  | shift_expr { $$ = move($1); }
  ;

eq_op
  : EQ  { $$ = BinOpExpr::EQ; }
  | NEQ { $$ = BinOpExpr::NEQ; }
  ;

eq_expr
  : eq_expr eq_op comp_expr { $$ = make_unique<BinOpExpr>($1, $3, $2); }
  | comp_expr { $$ = move($1); }
  ;

band_expr
  : band_expr BAND eq_expr { $$ = make_unique<BinOpExpr>($1, $3, BinOpExpr::BAND); }
  | eq_expr { $$ = move($1); }
  ;

bxor_expr
  : bxor_expr BXOR band_expr { $$ = make_unique<BinOpExpr>($1, $3, BinOpExpr::BXOR); }
  | band_expr { $$ = move($1); }
  ;

bor_expr
  : bor_expr BOR bxor_expr { $$ = make_unique<BinOpExpr>($1, $3, BinOpExpr::BOR); }
  | bxor_expr { $$ = move($1); }
  ;

land_expr
  : land_expr LAND bor_expr { $$ = make_unique<BinOpExpr>($1, $3, BinOpExpr::LAND); }
  | bor_expr { $$ = move($1); }
  ;

lxor_expr
  : lxor_expr LXOR land_expr { $$ = make_unique<BinOpExpr>($1, $3, BinOpExpr::LXOR); }
  | land_expr { $$ = move($1); }
  ;

lor_expr
  : lor_expr LOR lxor_expr { $$ = make_unique<BinOpExpr>($1, $3, BinOpExpr::LOR); }
  | lxor_expr { $$ = move($1); }
  ;

// expr_inner
//   : expr_inner SEMICOLON lor_expr { $1.push_back(move($3)); $$ = move($1); }
//   | expr_inner lor_expr { $1.push_back(move($2)); $$ = move($1); }
//   | lor_expr { vector<unique_ptr<Expr>> e; e.push_back(move($1)); $$ = move(e); }
//   ;

if_expr 
  : "if" expr "then" body "end" { $$ = make_unique<IfElseExpr>($2, $4); }
  | "if" expr "then" body "else" body "end" { $$ = make_unique<IfElseExpr>($2, $4, $6); }
  ;

case_ 
  : pat ARROW lor_expr { $$ = make_pair(move($1), move($3)); }
  ;

cases
  : cases "|" case_ { $1.push_back(move($3)); $$ = move($1); }
  | case_ { vector<Case> x; x.push_back(move($1)); $$ = move(x); }
  | "|" case_ { vector<Case> x; x.push_back(move($2)); $$ = move(x); }
  ;

case_expr
  : "case" expr "of" cases { $$ = make_unique<CaseExpr>($2, $4); }
  ;
expr
  : lor_expr { $$ = move($1); }
  | if_expr { $$ = move($1); }
  | case_expr { $$ = move($1); }
  ;

stmt_list
  : stmt_list stmt { $1.push_back(move($2)); $$ = move($1); }
  | stmt { vector<unique_ptr<St>> s; s.push_back(move($1)); $$ = move(s); }
  ;

program
  : stmt_list END { env.pt = make_unique<Program>($1); $$ = 0; }
  ;

%%

// Bison expects us to provide implementation - otherwise linker complains
void dasl::Parser::error(const location &loc , const std::string &message) {
        
        // Location should be initialized inside lexer action, but is not in this example.
        // Let's grab location directly from driver class.
	// cout << "Error: " << message << endl << "Location: " << loc << endl;
	
        cout << "Error: " << message << endl << "Error location: " << loc.begin.line << ":" << loc.begin.column << endl;
}
