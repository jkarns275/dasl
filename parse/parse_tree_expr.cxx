namespace dasl::pt {

Expr::Expr() {}
Expr::Expr(unique_ptr<Type> &type) : type(move(type)) {}

string body_to_string(const Body &body, Env &env) {
  string s;
  env.scope_start();
  for (auto it = body.begin(); it != body.end(); it++) { s += env.indent() + (*it)->to_string(env) + "\n"; }
  env.scope_end();
  return s;
}

void Expr::set_type(unique_ptr<Type> &type) {
  this->type = move(type);
}

IfElseExpr::IfElseExpr(unique_ptr<Expr> &cond, Body &body) : cond(move(cond)), body(move(body)), else_body(nullopt) {}
IfElseExpr::IfElseExpr(unique_ptr<Expr> &cond, Body &body, Body &else_body)
    : cond(move(cond)), body(move(body)), else_body(move(else_body)) {}

string IfElseExpr::to_string(Env &env) const {
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

CaseExpr::CaseExpr(unique_ptr<Expr> &value, vector<Case> &cases) : value(move(value)), cases(move(cases)) {}

string case_to_string(const Case &case_, Env &env) {
  string s = env.indent() + "| " + case_.first->to_string(env) + " => " + case_.second->to_string(env) + "\n";
  return s;
}

string CaseExpr::to_string(Env &env) const {
  string s = env.indent() + "case " + value->to_string(env) + " of\n";
  env.scope_start();
  for (auto it = cases.cbegin(); it != cases.cend(); it++) { s += case_to_string(*it, env); }
  env.scope_end();
  return s;
}

string record_expr_field_to_string(const RecordExprField &r, Env &env) {
  return r.first.to_string(env) + ": " + r.second->to_string(env);
}

RecordExpr::RecordExpr(SymbolRef &symbol) : name(move(symbol)) {}
RecordExpr::RecordExpr(SymbolRef &symbol, vector<RecordExprField> &fields) : name(move(symbol)), fields(move(fields)) {}

string RecordExpr::to_string(Env &env) const {
  string s = name.to_string(env) + " { ";
  for (auto it = fields.cbegin(); it != fields.cend(); it++) { s += record_expr_field_to_string(*it, env) + ", "; }
  if (fields.size() != 0) {
    s.pop_back();
    s.pop_back();
  }
  return s + " }";
}

ListExpr::ListExpr() {}
ListExpr::ListExpr(unique_ptr<Expr> &value) : value(move(value)) {}
ListExpr::ListExpr(unique_ptr<Expr> &value, unique_ptr<ListExpr> &tail)
    : value(move(value)), tail(move(tail)) {}

unique_ptr<ListExpr> ListExpr::make(vector<unique_ptr<Expr>> &values, unique_ptr<Expr> tail) {
  unique_ptr<ListExpr> tail_node = tail ? make_unique<ListExpr>(tail) : unique_ptr<ListExpr>();
  auto null_expr = unique_ptr<Expr>();
  unique_ptr<ListExpr> tail_container = make_unique<ListExpr>(null_expr, tail_node);
  unique_ptr<ListExpr> carry = move(tail_container);

  for (int i = values.size() - 1; i > 0; i--)
    carry = make_unique<ListExpr>(values[i], carry);

  return make_unique<ListExpr>(values[0], carry);
}

string ListExpr::to_string(Env &env) const {
  string s = "[";
  const ListExpr *i = this;
  while (i) {
    if (i->value) {
      s += i->value->to_string(env) + ", ";
      i = i->tail.get();
      if (i == nullptr || i->is_empty()) {
        s.pop_back();
        s.pop_back();
        break;
      }
    } else if (i->tail) {
      s.pop_back();
      s.pop_back();
      s += " :: " + i->tail->value->to_string(env);
      break;
    } else {
      break;
    }
  }
  return s + "]";
}

bool ListExpr::is_empty() const {
  return !value && !tail;
}

MapExpr::MapExpr() {}
MapExpr::MapExpr(vector<pair<unique_ptr<Expr>, unique_ptr<Expr>>> &items) : items(move(items)) {}

string MapExpr::to_string(Env &env) const {
  string s = "{ ";

  for (auto it = items.cbegin(); it != items.cend(); it++)
    s += (*it).first->to_string(env) + " => " + (*it).second->to_string(env) + ", ";

  if (items.size() != 0) {
    s.pop_back();
    s.pop_back();
  }

  return s + " }";
}

ValueExpr::ValueExpr() : value(Unit{}) {}
ValueExpr::ValueExpr(Value value) : value(move(value)) {}

string ValueExpr::to_string(Env &env) const {
  return value.to_string(env);
}

SymbolExpr::SymbolExpr(SymbolRef &symbol) : symbol(move(symbol)) {}

string SymbolExpr::to_string(Env &env) const { return symbol.to_string(env); }

CallExpr::CallExpr(SymbolRef &name) : name(move(name)) {}
CallExpr::CallExpr(SymbolRef &name, vector<unique_ptr<Expr>> &args) : name(move(name)), args(move(args)) {}

string CallExpr::to_string(Env &env) const {
  string s = name.to_string(env) + "(";
  for (auto it = args.cbegin(); it != args.cend(); it++) { s += (*it)->to_string(env) + ", "; }
  if (args.size() != 0) {
    s.pop_back();
    s.pop_back();
  }
  s += ")";
  return s;
}

BinOpExpr::BinOpExpr(unique_ptr<Expr> &lhs, unique_ptr<Expr> &rhs, BinOp op) : lhs(move(lhs)), rhs(move(rhs)), op(op) {}

string BinOpExpr::to_string(Env &env) const {
  string op_s;

  switch (op) {
    case ADD:
      op_s = "+";
      break;
    case SUB:
      op_s = "-";
      break;
    case MUL:
      op_s = "*";
      break;
    case DIV:
      op_s = "/";
      break;
    case MOD:
      op_s = "%";
      break;
    case LAND:
      op_s = "&&";
      break;
    case LOR:
      op_s = "||";
      break;
    case LXOR:
      op_s = "^^";
      break;
    case BAND:
      op_s = "band";
      break;
    case BOR:
      op_s = "bor";
      break;
    case BXOR:
      op_s = "bxor";
      break;
    case EQ:
      op_s = "==";
      break;
    case NEQ:
      op_s = "!=";
      break;
    case LSH:
      op_s = "<<";
      break;
    case RSH:
      op_s = ">>";
      break;
    case INDEX:
      op_s = "[]";
      break;
    case GT:
      op_s = ">";
      break;
    case GTE:
      op_s = ">=";
      break;
    case LT:
      op_s = "<";
      break;
    case LTE:
      op_s = "<=";
      break;
  }

  if (op == INDEX) return lhs->to_string(env) + "[" + rhs->to_string(env) + "]";

  return "(" + lhs->to_string(env) + " " + op_s + " " + rhs->to_string(env) + ")";
}

UnOpExpr::UnOpExpr(unique_ptr<Expr> &value, UnOp op) : value(move(value)), op(op) {}

string UnOpExpr::to_string(Env &env) const {
  string op_s;
  switch (op) {
    case NOT:
      op_s = "!";
      break;
    case NEG:
      op_s = "-";
      break;
    case INV:
      op_s = "~";
      break;
  }

  return op_s + value->to_string(env);
}

CompoundExpr::CompoundExpr(vector<unique_ptr<Expr>> &exprs) : exprs(move(exprs)) {}

string CompoundExpr::to_string(Env &env) const {
  string s;

  for (auto it = exprs.begin(); it != exprs.end(); it++) { s += (*it)->to_string(env) + "; "; }

  s.pop_back();
  s.pop_back();

  return s;
}

} // namespace dasl::pt
