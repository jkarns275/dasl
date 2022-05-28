#include "parse_tree.hxx"

using std::nullopt;
using std::move;

namespace dasl::pt {

PT::PT() {}
PT::~PT() {}

void PT::set_location(location &loc) { loc = loc; }

SymbolRef::SymbolRef(istring name) : name(name) {}
SymbolRef::SymbolRef(vector<istring> &modules, istring name) : name(name), modules(move(modules)) {}
string SymbolRef::to_string(Env &env) {
  string out;
  for (int i = 0; i < modules.size(); i++) {
    out.append(env.interner.get_string(modules[i]));
    out.append(".");
  }
  out.append(env.interner.get_string(name));
}

Type::Type() {}
Type::~Type() {}

ListType::ListType() {}
ListType::~ListType() {}
string ListType::to_string(Env &env) { return "list"; }

MapType::MapType() {}
MapType::~MapType() {}
string MapType::to_string(Env &env) { return "map"; }

RecordType::RecordType(SymbolRef &symbol) : symbol(move(symbol)) {}
RecordType::~RecordType() {}
string RecordType::to_string(Env &env) { return symbol.to_string(env); }

AnyType::AnyType() {}
AnyType::~AnyType() {}
string AnyType::to_string(Env &env) { return "any"; }

PrimType::PrimType(PrimKind kind) : kind(kind) {}
PrimType::~PrimType() {}
string PrimType::to_string(Env &env) {
  switch (kind) {
    case STRING:
      return "string";
    case INT:
      return "int";
    case FLOAT:
      return "float";
    case BOOL:
      return "bool";
    case ATOM:
      return "atom";
    case UNIT:
      return "()";
  }
}

Expr::Expr() {}
Expr::Expr(unique_ptr<Type> type) : type(move(type)) {}
Expr::~Expr() {}

ValueExpr::ValueExpr() : ValueExpr(Unit{}) {}
ValueExpr::ValueExpr(Value value) : value(value), kind(static_cast<ValueKind>(value.index())) {}
ValueExpr::~ValueExpr() {}

string ValueExpr::to_string(Env &env) {
  switch (kind) {
    case STRING:
      return "\"" + env.interner.get_string(std::get<StringValue>(value).str) + "\"";
    case UNIT:
      return "()";
    case INT:
      return std::to_string(std::get<size_t>(value));
    case FLOAT:
      return std::to_string(std::get<double>(value));
    case BOOL:
      if (std::get<bool>(value))
        return "true";
      else
        return "false";
    case ATOM:
      return env.interner.get_string(std::get<AtomValue>(value).atom);
    default:
      // Unreachable
      exit(1);
  }
}

string body_to_string(Body &body, Env &env) {
  env.scope_start();
  for (auto it = body.begin(); it != body.end(); it++) {
    s += env.indent() + (*it)->to_string() + "\n";
  }
  env.scope_end();
}

IfElseExpr::IfElseExpr(unique_ptr<Expr> cond, Body &body)
    : cond(move(cond)), body(move(body)), else_body(nullopt) {}
IfElseExpr::IfElseExpr(unique_ptr<Expr> cond, Body &body, Body &else_body)
    : cond(move(cond)), body(move(body)), else_body(move(else_body)) {}

virtual IfElseExpr::~IfElseExpr() {}
string IfElseExpr::to_string(Env &env) {
  string s = env.indent() + "if " + cond->to_string(env) + " then\n";
  s += body_to_string(body, env);
  if (else_body != nullopt) {
    s += env.indent() + "else\n";
    s += body_to_string(*else_body, env);
    s += env.indent() + "end\n";
  } else {
    s += env.indent() + "end\n";
  }
  return s;
}

CaseExpr::CaseExpr(unique_ptr<Expr> value, vector<Case> &cases) : value(move(value)), cases(move(cases)) {}
CaseExpr::~CaseExpr() {}

string case_to_string(Case &case_, Env &env) {
  string s = env.indent() + "| " + case_.first->to_string(env) + " => " + case_.second->to_string(env) + "\n";
  return s;
}

string CaseExpr::to_string(Env &env) {
  string s = env.indent() + "case " + value->to_string(env) + " of\n";
  env.scope_start();
  for (auto it = cases.begin(); it != cases.end(); it++) {
    s += case_to_string(*it, env);
  }
  env.scope_end();
}

string record_expr_field_to_string(RecordExprField &r, Env &env) {
  return r.first.to_string() + ": " + r.second->to_string();
}

RecordExpr::RecordExpr(SymbolRef &symbol) : name(move(symbol)) {}
RecordExpr::RecordExpr(SymbolRef &symbol, vector<RecordExprField> &fields) : name(move(symbol)), fields(move(fields)) {}
RecordExpr::~RecordExpr() {}

RecordExpr::to_string(Env &env) {
  string s = symbol->to_string(env) + " { ";
  for (auto it = fields.begin(); it != fields.end(); it++) {
    s += record_expr_field_to_string(*it, env) + ", ";
  }
  if (fields.size() != 0) {
    s.pop_back(); s.pop_back();
  }
  return s + " }";
}

ListExpr::ListExpr() {}
ListExpr::ListExpr(vector<unique_ptr<Expr>> &elements) : elements(move(elements)) {}
ListExpr::ListExpr(vector<unique_ptr<Expr>> &elements, unique_ptr<Expr> tail) : elements(move(elements)), tail(move(tail)) {}
ListExpr::~ListExpr() {}

string ListExpr::to_string(Env &env) {
  string s = "[";
  for (auto it = elements.begin(); it != elements.end(); it++) {
    s += (*it)->to_string(env) + ", ";
  }
  if (elements.size() != 0 || tail != nullopt) {
    s.pop_back(); s.pop_back();
  }
  if (tail != nullopt) {
    s += " :: " + (*tail)->to_string(env);
  }
  return s;
}


MapExpr::MapExpr() {}
MapExpr::MapExpr(vector<pair<unique_ptr<Expr>, unique_ptr<Expr>>> &items) : items(move(items)) {}
MapExpr::~MapExpr() {}

string MapExpr::to_string(Env &env) {
  string s = "{ ";

  for (auto it = fields.begin(); it != fields.end(); it++)
    s += (*it).first->to_string(env) + " => " + (*it).second->to_string(env) + ", ";

  if (fields.size() != 0) {
    s.pop_back(); s.pop_back();
  }

  return s + " }";
}

ValueExpr::ValueExpr() : value(Unit {}), kind(value.index()) {}
ValueExpr::ValueExpr(Value &value) : value(move(value)), kind(this->value.index()) {}
ValueExpr::~ValueExpr() {}

string ValueExpr::to_string(Env &env) {
  switch (kind) {
    case STRING:
      return "\"" + get<STRING>(value).to_string(env) + "\"";
    case UNIT:
      return "()";
    case INT:
      return to_string(get<INT>(value));
    case FLOAT:
      return to_string(get<FLOAT>(value));
    case BOOL:
      return to_string(get<BOOL>(value));
    case ATOM:
      return ":" + get<ATOM>(value).to_string(env);
    default:
      // Unreachable
      exit(1);
  }
}

SymbolExpr::SymbolExpr(SymbolRef &symbol) : symbol(move(symbol)) {}
SymbolExpr::~SymbolExpr() {}

string SymbolExpr::to_string(Env &env) {
  return symbol.to_string(env);
}

CallExpr::CallExpr(SymbolRef &name) : name(move(name)) {}
CallExpr::CallExpr(SymbolRef &name, vector<unique_ptr<Expr>> &args) : name(move(name)), args(move(args)) {}
CallExpr::~CallExpr();



}  // namespace dasl::pt
