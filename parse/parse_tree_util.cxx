namespace dasl::pt {

string Env::indent() {
  string s;
  for (int i = 0; i < depth; i++)
    s += "  ";
  return s;
}

void Env::scope_start() { depth += 1; }
void Env::scope_end() { depth -= 1; }

SymbolRef::SymbolRef(Id name) : name(name) {}
SymbolRef::SymbolRef(vector<Id> &modules, Id name) : name(name), modules(move(modules)) {}

SymbolRef &SymbolRef::operator=(SymbolRef&& other) {
  modules = move(other.modules);
  name = other.name;
}

void SymbolRef::shift(Id n) {
  modules.push_back(name);
  name = n;
}

string SymbolRef::to_string(Env &env) const {
  string out;
  for (int i = 0; i < modules.size(); i++) {
    out += modules[i].to_string(env);
    out += ".";
  }
  out += name.to_string(env);
  return out;
}

InternedValue::InternedValue() : val(istring { ~0UL }) {}
InternedValue::InternedValue(istring val) : val(val) {}
InternedValue::InternedValue(const InternedValue &iv) : val(iv.val) {}

string InternedValue::to_string(Env &env) const {
  return env.interner.get_string(val);
}

StringValue::StringValue(istring str) : InternedValue(str) {}
StringValue::StringValue(const InternedValue &iv) : InternedValue(iv.val) {}

AtomValue::AtomValue(istring atom) : InternedValue(atom) {}
AtomValue::AtomValue(const InternedValue &iv) : InternedValue(iv.val) {}

Id::Id(istring id) : InternedValue(id) {}
Id::Id(const InternedValue &iv) : InternedValue(iv.val) {}

Unit::Unit() {}
string Unit::to_string(Env &env) const { return "{}"; }

Value::Value(ValueType value) : value(value), kind(static_cast<ValueKind>(this->value.index())) {}
string Value::to_string(Env &env) const {
  switch (kind) {
    case STRING:
      return "\"" + std::get<STRING>(value).to_string(env) + "\"";
    case UNIT:
      return "()";
    case INT:
      return std::to_string(std::get<INT>(value));
    case FLOAT:
      return std::to_string(std::get<FLOAT>(value));
    case BOOL:
      return std::to_string(std::get<BOOL>(value));
    case ATOM:
      return ":" + std::get<ATOM>(value).to_string(env);
    default:
      // Unreachable
      exit(1);
  }
}

} // namespace dasl::pt
