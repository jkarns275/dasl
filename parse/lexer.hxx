#ifndef LEXER_HXX
#define LEXER_HXX

#if !defined(yyFlexLexerOnce)
#undef yyFlexLexer
#define yyFlexLexer dasl_FlexLexer // the trick with prefix; no namespace here :(
#include <FlexLexer.h>
#endif

// Scanner method signature is defined by this macro. Original yylex() returns int.
// Sinice Bison 3 uses symbol_type, we must change returned type. We also rename it
// to something sane, since you cannot overload return type.
#undef YY_DECL
#define YY_DECL dasl::Parser::symbol_type dasl::Lexer::get_next_token()

#include <parser.hxx>
#include <location.hh>

namespace dasl {

class Lexer : public yyFlexLexer {
 public:
  Lexer() {}
  virtual ~Lexer() {}
  dasl::Parser::symbol_type get_next_token();

  void increase_location(int i) { m_location += i; }

  int m_location = 0;
  dasl::location location;
};

} // namespace dasl

#endif // LEXER_HXX
