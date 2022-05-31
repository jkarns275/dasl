%{
  #include <iostream>
  #include <cstdlib>
  #include "lexer.hxx"
  #include "parser.hxx"
  #include "location.hh"

  using namespace std;

#define YY_USER_ACTION  loc.columns (yyleng);

#define span() loc

#define yyterminate() dasl::Parser::make_END(span());

%}

%option nodefault
%option noyywrap
%option c++
%option yyclass="Lexer"
%option prefix="dasl_"

%%

%{
  // A handy shortcut to the location held by the driver.
  dasl::location& loc = this->location;
  // Code run each time yylex is called.
  loc.step ();
%}

:[a-zA-Z_0-9'?]+ {
  return dasl::Parser::make_ATOM(yytext, span());
}

::      { return dasl::Parser::make_TAIL(span()); }

:       { return dasl::Parser::make_COLON(span()); }

end     { return dasl::Parser::make_KW_END(span()); }

def     { return dasl::Parser::make_KW_DEF(span()); }

module  { return dasl::Parser::make_KW_MODULE(span()); }

if      { return dasl::Parser::make_KW_DO(span()); }

if      { return dasl::Parser::make_KW_IF(span()); }
then    { return dasl::Parser::make_KW_THEN(span()); }
else    { return dasl::Parser::make_KW_ELSE(span()); }

for     { return dasl::Parser::make_KW_FOR(span()); }
in      { return dasl::Parser::make_KW_IN(span()); }

case    { return dasl::Parser::make_KW_CASE(span()); }
of      { return dasl::Parser::make_KW_OF(span()); }

type    { return dasl::Parser::make_KW_TYPE(span()); }

int     { return dasl::Parser::make_KW_INT(span()); }
float   { return dasl::Parser::make_KW_FLOAT(span()); }
bool    { return dasl::Parser::make_KW_BOOL(span()); }
true    { return dasl::Parser::make_KW_TRUE(span()); }
false   { return dasl::Parser::make_KW_FALSE(span()); }
atom    { return dasl::Parser::make_KW_ATOM(span()); }
any     { return dasl::Parser::make_KW_ANY(span()); }
string  { return dasl::Parser::make_KW_STRING(span()); }
map     { return dasl::Parser::make_KW_MAP(span()); }
list    { return dasl::Parser::make_KW_LIST(span()); }

\[      { return dasl::Parser::make_SQ_BOPEN(span()); }
\]      { return dasl::Parser::make_SQ_BCLOSE(span()); }
\{      { return dasl::Parser::make_CBOPEN(span()); }
\}      { return dasl::Parser::make_CBCLOSE(span()); }

\.      { return dasl::Parser::make_DOT(span()); }
\+      { return dasl::Parser::make_ADD(span()); }
-       { return dasl::Parser::make_SUB(span()); }
\*      { return dasl::Parser::make_MUL(span()); }
\/      { return dasl::Parser::make_DIV(span()); }
%       { return dasl::Parser::make_MOD(span()); }

>>      { return dasl::Parser::make_RSH(span()); }
\<\<    { return dasl::Parser::make_LSH(span()); }


&&      { return dasl::Parser::make_LAND(span()); }
\|\|    { return dasl::Parser::make_LOR(span()); }
\^\^    { return dasl::Parser::make_LXOR(span()); }

=>      { return dasl::Parser::make_ARROW(span()); }
==      { return dasl::Parser::make_EQ(span()); }
!=      { return dasl::Parser::make_NEQ(span()); }
>=      { return dasl::Parser::make_GTE(span()); }
>       { return dasl::Parser::make_GT(span()); }
\<=     { return dasl::Parser::make_LTE(span()); }
\<      { return dasl::Parser::make_LT(span()); }

and     { return dasl::Parser::make_BAND(span()); }
or      { return dasl::Parser::make_BOR(span()); }
xor     { return dasl::Parser::make_BXOR(span()); }

!       { return dasl::Parser::make_NOT(span()); }
~       { return dasl::Parser::make_INV(span()); }

=       { return dasl::Parser::make_ASSIGN(span()); }

\"(\\.|[^"\\])*\" {
  return dasl::Parser::make_STRING(std::string(yytext + 1, yytext + yyleng - 1), span());
}

[a-zA-Z_'?][a-zA-Z0-9'_?]* {
  // TOOD intern this
  return dasl::Parser::make_ID(yytext, span()); 
}

\( {
  return dasl::Parser::make_POPEN(span());
}
            
\) { 
  return dasl::Parser::make_PCLOSE(span());
}

, {
  return dasl::Parser::make_COMMA(span());
}
            
[\n\t ] {
  //cout << "Scanner: whitechar (ignored)" << endl;
}

[1-9][0-9]* {
  uint64_t number = strtoull(yytext, 0, 10);
  return dasl::Parser::make_INT(number, span());
}

0x[0-9a-fA-F]+ {
  uint64_t number = strtoull(yytext + 2, nullptr, 16);
  return dasl::Parser::make_INT(number, span());
}

0[0-7]+ {
  uint64_t number = strtoull(yytext + 2, nullptr, 8);
  return dasl::Parser::make_INT(number, span());
}

[0-9]*([0-9]\.?|\.[0-9])[0-9]*([Ee][-+]?[0-9]+) {
  double f = strtod(yytext, nullptr);
  return dasl::Parser::make_FLOAT(f, span());
}

. { 
  cout << "Scanner: unknown character [" << yytext << "]" << endl; 
}
            
<<EOF>> { return yyterminate(); }


%%
