namespace dasl::pt {

DefSt::DefSt(Id name, vector<unique_ptr<Pat>> &args, vector<unique_ptr<St>> &body)
    : name(name), args(move(args)), body(move(body)) {}
DefSt::DefSt(Id name, vector<unique_ptr<Pat>> &args, unique_ptr<Type> &type, vector<unique_ptr<St>> &body)
    : name(name), args(move(args)), type(move(type)), body(move(body)) {}

string DefSt::to_string(Env &env) const {
  string s = env.indent() +  "def " + name.to_string(env) + "(";

  if (args.size()) {
    for (auto it = args.begin(); it != args.end(); it++)
      s += (*it)->to_string(env) + ", ";
    s.pop_back(); s.pop_back();
  }

  s += ")";
  if (type) {
    s += " => " + type->to_string(env) + ":\n";
  } else {
    s += ":\n";
  }

  env.scope_start();

  for (auto it = body.begin(); it != body.end(); it++) {
    s += env.indent() + (*it)->to_string(env) + ";\n";
  }
  s.pop_back();
  s.pop_back();
  s += "\n";

  env.scope_end();

  s += env.indent() + "end\n";
  return s;
}

string record_entry_to_string(const RecordEntry &entry, Env &env) {
  return entry.first.to_string(env) + ": " + entry.second->to_string(env);
}

RecordSt::RecordSt(Id name) : name(name) {}
RecordSt::RecordSt(Id name, vector<RecordEntry> &fields) : name(name), fields(move(fields)) {}

string RecordSt::to_string(Env &env) const {
  string s = "type " + name.to_string(env) + " = ";
  if (fields.size()) {
    s += "{ ";

    for (auto it = fields.begin(); it != fields.end(); it++)
      s += record_entry_to_string(*it, env) + ", ";

    s.pop_back(); s.pop_back();
    return s + " }";
  } else {
    return s + "{}";
  }
}

ValSt::ValSt(Id name, unique_ptr<Expr> &expr) : name(name), expr(move(expr)) {}

string ValSt::to_string(Env &env) const {
  return "val " + name.to_string(env) + " = " + expr->to_string(env);
}

ModuleSt::ModuleSt(Id name, vector<unique_ptr<St>> &statements) : name(name), statements(move(statements)) {}

string ModuleSt::to_string(Env &env) const {
  string s = env.indent() + "module " + name.to_string(env) + "\n";

  env.scope_start();

  for (auto it = statements.begin(); it != statements.end(); it++)
    s += env.indent() + (*it)->to_string(env) + "\n";

  env.scope_end();

  s += env.indent() + "end\n";

  return s;
}

ExprSt::ExprSt(unique_ptr<Expr> &expr) : expr(move(expr)) {}
string ExprSt::to_string(Env &env) const {
  return expr->to_string(env);
}

} // namespace dasl::pt
