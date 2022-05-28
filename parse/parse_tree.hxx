#ifndef PARSE_TREE_HXX
#define PARSE_TREE_HXX

#include <variant>
using std::variant;

#include <utility>
using std::pair;

#include <vector>
using std::vector;

#include <string>
using std::string;
#include <memory>
using std::unique_ptr;

#include <optional>
using std::optional;

#include <unordered_map>
using std::unordered_map;

#include "location.hh"
using dasl::location;
using dasl::position;

#include "interner.hxx"
using dasl::Interner;
using dasl::istring;

namespace dasl::pt {

struct Env {
  Interner interner;
};

struct PT {
  location loc;

  PT();
  virtual ~PT();

  virtual string to_string(Env &env) = 0;
  void set_location(location &loc);
};

struct SymbolRef : public PT {
  const istring name;
  const vector<istring> modules;

  SymbolRef();
  explicit SymbolRef(istring name);
  explicit SymbolRef(vector<istring>& modules, istring name);
  SymbolRef(SymbolRef &&) = default;

  SymbolRef &operator=(SymbolRef&& other);

  void shift(istring n);

  string to_string(Env &env) override;
};

struct Type : public PT {
  Type();
  virtual ~Type();
};

struct ListType : public Type {
  ListType();
  virtual ~ListType();

  string to_string(Env &env) override;
};

struct MapType : public Type {
  MapType();
  virtual ~MapType();

  string to_string(Env &env) override;
};

struct RecordType : public Type {
  SymbolRef symbol;
  // Type spec will have these fields.
  // const unordered_map<istring, unique_ptr<Type>> fields;

  explicit RecordType(SymbolRef &symbol);
  virtual ~RecordType();

  string to_string(Env &env) override;
};

struct AnyType : public Type {
  AnyType();
  virtual ~AnyType();

  string to_string(Env &env) override;
};

struct PrimType : public Type {
  const enum PrimKind { STRING, INT, FLOAT, BOOL, ATOM, UNIT } kind;

  explicit PrimType(PrimKind kind);
  virtual ~PrimType();

  string to_string(Env &env) override;
};

struct Pat : public PT {
  const optional<unique_ptr<Type>> type;

  Pat();
  explicit Pat(unique_ptr<Type> type);
  virtual ~Pat();

  void set_type(unique_ptr<Type> type);
};

struct ListPat : public Pat {
  const vector<unique_ptr<Pat>> members;
  const optional<unique_ptr<Pat>> tail;

  ListPat();
  ListPat(vector<unique_ptr<Pat>> &members);
  ListPat(vector<unique_ptr<Pat>> &members, unique_ptr<Pat> tail);
  virtual ~ListPat();

  string to_string(Env &env) override;
};

typedef pair<unique_ptr<Pat>, unique_ptr<Pat>> MapPatEntry;
struct MapPat : public Pat {
  const vector<MapPatEntry> entries;

  MapPat();
  explicit MapPat(vector<pair<unique_ptr<Pat>, unique_ptr<Pat>>> &entries);
  virtual ~MapPat();

  string to_string(Env &env) override;
};

typedef pair<istring, unique_ptr<Pat>> RecordPatField;
struct RecordPat : public Pat {
  const unordered_map<istring, unique_ptr<Pat>> fields;

  explicit RecordPat(SymbolRef& record_name);
  RecordPat(SymbolRef &record_name, vector<RecordPatField> &fields);
  virtual ~RecordPat();

  string to_string(Env &env) override;
};

struct SymbolPat : public Pat {
  // If this is none it is just a wild card _
  SymbolRef symbol;

  explicit SymbolPat(SymbolRef &symbol);
  virtual ~SymbolPat();

  string to_string(Env &env) override;
};

struct Unit : public PT {
  string to_string(Env &env) override;
};
struct AtomValue : public PT {
  istring atom;

  string to_string(Env &env) override;
};

struct StringValue {
  istring str;

  string to_string(Env &env) override;
};

enum ValueKind { STRING, UNIT, INT, FLOAT, BOOL, ATOM };
typedef variant<StringValue, Unit, std::size_t, double, bool, AtomValue> Value;

struct ValuePat : public Pat {
  const ValueKind kind;
  Value value;

  explicit ValuePat(Value value);
  virtual ~ValuePat();

  string to_string(Env &env) override;
};

struct Expr : public PT {
  // Will be nullopt if type is not explicit or trivially inferrable.
  // More sophisticated type inference will occur in the IR.
  const optional<unique_ptr<Type>> type;

  Expr();
  explicit Expr(unique_ptr<Type> type);
  virtual ~Expr();
};

class St;

// TODO: Make these PT classes
typedef vector<unique_ptr<St>> Body;

string body_to_string(Body &, Env &);

struct IfElseExpr : public Expr {
  const unique_ptr<Expr> cond;
  const Body body;
  const optional<Body> else_body;

  IfElseExpr(unique_ptr<Expr> cond, Body &body);
  IfElseExpr(unique_ptr<Expr> cond, Body &body, Body &else_body);
  virtual ~IfElseExpr();

  string to_string(Env &env) override;
};

typedef pair<unique_ptr<Pat>, unique_ptr<Expr>> Case;

string case_to_string(Case &case_, Env &env);

struct CaseExpr : public Expr {
  const unique_ptr<Expr> value;
  const vector<Case> cases;

  CaseExpr(unique_ptr<Expr> value, vector<Case> &cases);
  virtual ~CaseExpr();

  string to_string(Env &env) override;
};

typedef pair<AtomValue, unique_ptr<Expr>> RecordExprField;
string record_expr_field_to_string(RecordExprField &r, Env &env);

struct RecordExpr : public Expr {
  const SymbolRef name;
  const vector<RecordExprField> fields;

  explicit RecordExpr(SymbolRef &name);
  RecordExpr(SymbolRef &name, vector<RecordExprField> &fields);
  virtual ~RecordExpr();

  string to_string(Env &env) override;
};

struct ListExpr : public Expr {
  const vector<unique_ptr<Expr>> elements;
  const optional<unique_ptr<Expr>> tail;

  ListExpr();
  explicit ListExpr(vector<unique_ptr<Expr>> &elements);
  ListExpr(vector<unique_ptr<Expr>> &elements, unique_ptr<Expr> tail);
  virtual ~ListExpr();

  string to_string(Env &env) override;
};

struct MapExpr : public Expr {
  const vector<pair<unique_ptr<Expr>, unique_ptr<Expr>>> items;

  MapExpr();
  explicit MapExpr(vector<pair<unique_ptr<Expr>, unique_ptr<Expr>>> &items);
  virtual ~MapExpr();

  string to_string(Env &env) override;
};

// string, unit, int, float, bool, atom
struct ValueExpr : public Expr {
  const Value value;
  const ValueKind kind;

  // Unit
  ValueExpr();
  explicit ValueExpr(Value value);
  virtual ~ValueExpr();

  string to_string(Env &env) override;
};

struct SymbolExpr : public Expr {
  const SymbolRef symbol;

  explicit SymbolExpr(SymbolRef &symbol);
  virtual ~SymbolExpr();

  string to_string(Env &env) override;
};

struct CallExpr : public Expr {
  const SymbolRef name;
  const vector<unique_ptr<Expr>> args;

  CallExpr(SymbolRef &name, vector<unique_ptr<Expr>> &args);
  virtual ~CallExpr();

  string to_string(Env &env) override;
};

// add sub mul div mod AND OR XOR EQ NEQ lsh rsh, index
struct BinOpExpr : public Expr {
  const unique_ptr<Expr> lhs, rhs;
  enum BinOp { ADD, SUB, MUL, DIV, MOD, LAND, LOR, LXOR, BAND, BOR, BXOR, EQ, NEQ, LSH, RSH, INDEX, GT, GTE, LT, LTE } op;

  BinOpExpr(unique_ptr<Expr> lhs, unique_ptr<Expr> rhs, BinOp op);
  virtual ~BinOpExpr();

  string to_string(Env &env) override;
};

// not, inv
struct UnOpExpr : public Expr {
  const unique_ptr<Expr> value;
  enum UnOp { NOT, INV, NEG } op;

  UnOpExpr(unique_ptr<Expr> value, UnOp op);
  virtual ~UnOpExpr();

  string to_string(Env &env) override;
};

// List of exprs, introduces a new scope.
struct CompoundExpr : public Expr {
  const vector<unique_ptr<Expr>> exprs;

  explicit CompoundExpr(vector<unique_ptr<Expr>> &exprs);
  virtual ~CompoundExpr();

  string to_string(Env &env) override;
};

// Statements
struct St : public PT {
  St();
  virtual ~St();
};

// Defines a functino. Lambdas not supported so this returns type UNIT.
struct DefSt : public St {
  const istring name;
  const vector<unique_ptr<Pat>> args;
  const optional<unique_ptr<Type>> type;
  const vector<unique_ptr<St>> body;

  DefSt(istring name, vector<unique_ptr<Pat>> &args, vector<unique_ptr<St>> &body);
  DefSt(istring name, vector<unique_ptr<Pat>> &args, unique_ptr<Type> type, vector<unique_ptr<St>> &body);
  virtual ~DefSt();

  string to_string(Env &env) override;
};

typedef pair<AtomValue, unique_ptr<Type>> RecordEntry;
struct RecordSt : public St {
  const istring name;
  const vector<RecordEntry> fields;

  explicit RecordSt(istring name);
  RecordSt(istring name, vector<RecordEntry> &fields);
  virtual ~RecordSt();

  string to_string(Env &env) override;
};

struct ValSt : public St {
  const istring name;
  const unique_ptr<Expr> expr;

  ValSt(istring name, unique_ptr<Expr> expr);
  virtual ~ValSt();

  string to_string(Env &env) override;
};

struct ModuleSt : public St {
  const vector<unique_ptr<St>> statements;

  explicit ModuleSt(vector<unique_ptr<St>> &statements);
  virtual ~ModuleSt();

  string to_string(Env &env) override;
};

struct ExprSt : public St {
  const unique_ptr<Expr> st;

  explicit ExprSt(unique_ptr<Expr> expr);
  virtual ~ExprSt();

  string to_string(Env &env) override;
};

// This should always return UNIT
struct ForSt : public St {
  const unique_ptr<Pat> pattern;
  const unique_ptr<Expr> container;

  ForSt(unique_ptr<Expr> pattern, unique_ptr<Expr> container);
  virtual ~ForSt();

  string to_string(Env &env) override;
};

struct Program : public ModuleSt {
  explicit Program(vector<unique_ptr<St>> &statements);
  virtual ~Program();

  string to_string(Env &env) override;
};

} // namespace dasl::pt

#endif // PARSE_TREE_HXX
