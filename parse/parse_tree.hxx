#ifndef PARSE_TREE_HXX
#define PARSE_TREE_HXX

#include <variant>
using std::variant;

#include <utility>
using std::pair;
using std::make_pair;

#include <vector>
using std::vector;

#include <string>
using std::string;
#include <memory>
using std::unique_ptr;
using std::make_unique;

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

class Program;
struct Env {
  Interner interner;
  unique_ptr<Program> pt;
  int depth = 0;

  string indent();
  void scope_start();
  void scope_end();
};

struct PT {
  location loc;

  PT();
  virtual ~PT() = default;

  virtual string to_string(Env &env) const = 0;
  void set_location(location &loc);
};

struct Unit : public PT {
  Unit();
  virtual ~Unit() = default;

  string to_string(Env &env) const override;
};

struct InternedValue : public PT {
  istring val;

  InternedValue();
  explicit InternedValue(istring val);
  explicit InternedValue(const InternedValue &);
  virtual ~InternedValue() = default;

  string to_string(Env &env) const override;
};

struct AtomValue : public InternedValue {
  AtomValue() = default;
  explicit AtomValue(istring val);
  explicit AtomValue(const InternedValue &);
  virtual ~AtomValue() = default;
};

struct StringValue : public InternedValue {
  StringValue() = default;
  explicit StringValue(istring str);
  explicit StringValue(const InternedValue &);
  virtual ~StringValue() = default;
};

struct Id : public InternedValue {
  Id() = default;
  explicit Id(istring str);
  explicit Id(const InternedValue &);
  virtual ~Id() = default;
};

enum ValueKind { STRING, UNIT, INT, FLOAT, BOOL, ATOM };
typedef variant<StringValue, Unit, std::size_t, double, bool, AtomValue> ValueType;

struct Value : public PT {
  ValueType value;
  ValueKind kind;

  explicit Value(ValueType val);
  virtual ~Value() = default;

  string to_string(Env &env) const override;
};

struct SymbolRef : public PT {
  Id name;
  vector<Id> modules;

  SymbolRef() = default;
  explicit SymbolRef(Id name);
  explicit SymbolRef(vector<Id>& modules, Id name);
  SymbolRef(SymbolRef &&) = default;

  SymbolRef &operator=(SymbolRef&& other);

  void shift(Id n);

  string to_string(Env &env) const override;
};

struct Type : public PT {
  Type();
  virtual ~Type() = default;
};

struct ListType : public Type {
  ListType();
  virtual ~ListType() = default;

  string to_string(Env &env) const override;
};

struct MapType : public Type {
  MapType();
  virtual ~MapType() = default;

  string to_string(Env &env) const override;
};

struct RecordType : public Type {
  SymbolRef symbol;
  // Type spec will have these fields.
  // const unordered_map<istring, unique_ptr<Type>> fields;

  explicit RecordType(SymbolRef &symbol);
  virtual ~RecordType() = default;

  string to_string(Env &env) const override;
};

struct AnyType : public Type {
  AnyType();
  virtual ~AnyType() = default;

  string to_string(Env &env) const override;
};

struct PrimType : public Type {
  const enum PrimKind { STRING, INT, FLOAT, BOOL, ATOM, UNIT } kind;

  explicit PrimType(PrimKind kind);
  virtual ~PrimType() = default;

  string to_string(Env &env) const override;
};

struct Pat : public PT {
  unique_ptr<Type> type;

  Pat();
  explicit Pat(unique_ptr<Type> &type);
  virtual ~Pat() = default;

  void set_type(unique_ptr<Type> &type);
 
 protected:
  string type_string(Env &env) const;
};


struct ListPat : public Pat {
  // if pat && tail then this is a normal node in the list
  // if pat && !tail then this is the end of the list
  // if !pat && tail then this value in tail is supposed to represent a tail pattern
  // if !pat && !tail then the list is over! Can be used in place of a null tail value
  const unique_ptr<Pat> pat;
  const unique_ptr<ListPat> tail;


  ListPat();
  explicit ListPat(unique_ptr<Pat> &pat);
  ListPat(unique_ptr<Pat> &pat, unique_ptr<ListPat> &tail);
  static unique_ptr<ListPat> make(vector<unique_ptr<Pat>> &pats, unique_ptr<Pat> tail = unique_ptr<Pat>());
  virtual ~ListPat() = default;

  string to_string(Env &env) const override;
};

typedef pair<unique_ptr<Pat>, unique_ptr<Pat>> MapPatEntry;
struct MapPat : public Pat {
  const vector<MapPatEntry> entries;

  MapPat();
  explicit MapPat(vector<pair<unique_ptr<Pat>, unique_ptr<Pat>>> &entries);
  virtual ~MapPat() = default;

  string to_string(Env &env) const override;
};

typedef pair<AtomValue, unique_ptr<Pat>> RecordPatField;
struct RecordPat : public Pat {
  SymbolRef record_name;
  const vector<RecordPatField> fields;

  explicit RecordPat(SymbolRef& record_name);
  RecordPat(SymbolRef &record_name, vector<RecordPatField> &fields);
  virtual ~RecordPat() = default;

  string to_string(Env &env) const override;
};

struct SymbolPat : public Pat {
  // If this is none it is just a wild card _
  SymbolRef symbol;

  explicit SymbolPat(SymbolRef &symbol);
  virtual ~SymbolPat() = default;

  string to_string(Env &env) const override;
};

struct ValuePat : public Pat {
  Value value;

  explicit ValuePat(Value value);
  virtual ~ValuePat() = default;

  string to_string(Env &env) const override;
};

struct Expr : public PT {
  unique_ptr<Type> type;

  Expr();
  explicit Expr(unique_ptr<Type> &type);
  virtual ~Expr() = default;

  void set_type(unique_ptr<Type> &type);
};

class St;

// TODO: Make these PT classes
typedef vector<unique_ptr<St>> Body;

string body_to_string(Body &, Env &);

struct IfElseExpr : public Expr {
  const unique_ptr<Expr> cond;
  const Body body;
  const optional<Body> else_body;

  IfElseExpr(unique_ptr<Expr> &cond, Body &body);
  IfElseExpr(unique_ptr<Expr> &cond, Body &body, Body &else_body);
  virtual ~IfElseExpr() = default;

  string to_string(Env &env) const override;
};

typedef pair<unique_ptr<Pat>, unique_ptr<Expr>> Case;

string case_to_string(Case &case_, Env &env);

struct CaseExpr : public Expr {
  const unique_ptr<Expr> value;
  const vector<Case> cases;

  CaseExpr(unique_ptr<Expr> &value, vector<Case> &cases);
  virtual ~CaseExpr() = default;

  string to_string(Env &env) const override;
};

typedef pair<AtomValue, unique_ptr<Expr>> RecordExprField;
string record_expr_field_to_string(RecordExprField &r, Env &env);

struct RecordExpr : public Expr {
  const SymbolRef name;
  const vector<RecordExprField> fields;

  explicit RecordExpr(SymbolRef &name);
  RecordExpr(SymbolRef &name, vector<RecordExprField> &fields);
  virtual ~RecordExpr() = default;

  string to_string(Env &env) const override;
};

struct ListExpr : public Expr {
  const unique_ptr<Expr> value;
  const unique_ptr<ListExpr> tail;

  ListExpr();
  explicit ListExpr(unique_ptr<Expr> &value);
  ListExpr(unique_ptr<Expr> &value, unique_ptr<ListExpr> &tail);
  static unique_ptr<ListExpr> make(vector<unique_ptr<Expr>> &values, unique_ptr<Expr> tail = unique_ptr<Expr>());
  virtual ~ListExpr() = default;

  string to_string(Env &env) const override;
  bool is_empty() const;
};

struct MapExpr : public Expr {
  const vector<pair<unique_ptr<Expr>, unique_ptr<Expr>>> items;

  MapExpr();
  explicit MapExpr(vector<pair<unique_ptr<Expr>, unique_ptr<Expr>>> &items);
  virtual ~MapExpr() = default;

  string to_string(Env &env) const override;
};

// string, unit, int, float, bool, atom
struct ValueExpr : public Expr {
  const Value value;

  ValueExpr();
  explicit ValueExpr(Value value);
  virtual ~ValueExpr() = default;

  string to_string(Env &env) const override;
};

struct SymbolExpr : public Expr {
  const SymbolRef symbol;

  explicit SymbolExpr(SymbolRef &symbol);
  virtual ~SymbolExpr() = default;

  string to_string(Env &env) const override;
};

struct CallExpr : public Expr {
  const SymbolRef name;
  const vector<unique_ptr<Expr>> args;

  explicit CallExpr(SymbolRef &name);
  CallExpr(SymbolRef &name, vector<unique_ptr<Expr>> &args);
  virtual ~CallExpr() = default;

  string to_string(Env &env) const override;
};

// add sub mul div mod AND OR XOR EQ NEQ lsh rsh, index
struct BinOpExpr : public Expr {
  const unique_ptr<Expr> lhs, rhs;
  enum BinOp { ADD, SUB, MUL, DIV, MOD, LAND, LOR, LXOR, BAND, BOR, BXOR, EQ, NEQ, LSH, RSH, INDEX, GT, GTE, LT, LTE } op;

  BinOpExpr(unique_ptr<Expr> &lhs, unique_ptr<Expr> &rhs, BinOp op);
  virtual ~BinOpExpr() = default;

  string to_string(Env &env) const override;
};

// not, inv
struct UnOpExpr : public Expr {
  const unique_ptr<Expr> value;
  enum UnOp { NOT, INV, NEG } op;

  UnOpExpr(unique_ptr<Expr> &value, UnOp op);
  virtual ~UnOpExpr() = default;

  string to_string(Env &env) const override;
};

// List of exprs, introduces a new scope.
struct CompoundExpr : public Expr {
  const vector<unique_ptr<Expr>> exprs;

  explicit CompoundExpr(vector<unique_ptr<Expr>> &exprs);
  virtual ~CompoundExpr() = default;

  string to_string(Env &env) const override;
};

// Statements
struct St : public PT {
  St() = default;
  virtual ~St() = default;
};

// Defines a functino. Lambdas not supported so this returns type UNIT.
struct DefSt : public St {
  const Id name;
  const vector<unique_ptr<Pat>> args;
  const unique_ptr<Type> type;
  const vector<unique_ptr<St>> body;

  DefSt(Id name, vector<unique_ptr<Pat>> &args, vector<unique_ptr<St>> &body);
  DefSt(Id name, vector<unique_ptr<Pat>> &args, unique_ptr<Type> &type, vector<unique_ptr<St>> &body);
  virtual ~DefSt() = default;

  string to_string(Env &env) const override;
};

typedef pair<AtomValue, unique_ptr<Type>> RecordEntry;

string record_entry_to_string(const RecordEntry &entry, Env &env);

struct RecordSt : public St {
  const Id name;
  const vector<RecordEntry> fields;

  explicit RecordSt(Id name);
  RecordSt(Id name, vector<RecordEntry> &fields);
  virtual ~RecordSt() = default;

  string to_string(Env &env) const override;
};

struct ValSt : public St {
  const Id name;
  const unique_ptr<Expr> expr;

  ValSt(Id name, unique_ptr<Expr> &expr);
  virtual ~ValSt() = default;

  string to_string(Env &env) const override;
};

struct ModuleSt : public St {
  Id name;
  const vector<unique_ptr<St>> statements;

  ModuleSt() = default;
  explicit ModuleSt(Id name, vector<unique_ptr<St>> &statements);
  virtual ~ModuleSt() = default;

  string to_string(Env &env) const override;
};

struct ExprSt : public St {
  const unique_ptr<Expr> expr;

  explicit ExprSt(unique_ptr<Expr> &expr);
  virtual ~ExprSt() = default;

  string to_string(Env &env) const override;
};

// This should always return UNIT
struct ForSt : public St {
  const unique_ptr<Pat> pattern;
  const unique_ptr<Expr> container;

  ForSt(unique_ptr<Expr> &pattern, unique_ptr<Expr> &container);
  virtual ~ForSt() = default;

  string to_string(Env &env) const override;
};

struct Program : public ModuleSt {
  vector<unique_ptr<St>> statements;

  explicit Program(vector<unique_ptr<St>> &statements);
  virtual ~Program() = default;

  string to_string(Env &env) const override;
};

} // namespace dasl::pt

#endif // PARSE_TREE_HXX
