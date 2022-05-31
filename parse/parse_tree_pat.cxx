namespace dasl::pt {

Pat::Pat() {}
Pat::Pat(unique_ptr<Type> &type) : type(move(type)) {}

void Pat::set_type(unique_ptr<Type> &type) {
  this->type = move(type);
}

string Pat::type_string(Env &env) const {
  if (type)
    return ": " + type->to_string(env);
  else
    return "";
}

ListPat::ListPat() {}
ListPat::ListPat(unique_ptr<Pat> &pat) : pat(move(pat)) {}
ListPat::ListPat(unique_ptr<Pat> &pat, unique_ptr<ListPat> &tail) : pat(move(pat)), tail(move(tail)) {}

unique_ptr<ListPat> ListPat::make(vector<unique_ptr<Pat>> &pats, unique_ptr<Pat> tail) {
  // Will be final node if there is no tail!
  unique_ptr<ListPat> tail_node = tail ? make_unique<ListPat>(tail) : unique_ptr<ListPat>();
  auto null_pat = unique_ptr<Pat>();
  unique_ptr<ListPat> tail_container = make_unique<ListPat>(null_pat, tail_node);
  unique_ptr<ListPat> carry = move(tail_container);

  for (int i = pats.size() - 1; i > 0; i--)
    carry = make_unique<ListPat>(pats[i], carry);

  return make_unique<ListPat>(pats[0], carry);
}

string ListPat::to_string(Env &env) const {
  string s = "[";
  const ListPat *i = this;
  while (i) {
    if (i->pat) {
      s += i->pat->to_string(env) + ", ";
      i = i->tail.get();
      if (i == nullptr) {
        s.pop_back();
        s.pop_back();
        break;
      }
    } else if (i->tail) {
      s.pop_back();
      s.pop_back();
      s += " :: " + i->tail->pat->to_string(env);
      break;
    }
  }
  return s + "]" + type_string(env);
}

MapPat::MapPat() {}
MapPat::MapPat(vector<pair<unique_ptr<Pat>, unique_ptr<Pat>>> &entries) : entries(move(entries)) {}

string MapPat::to_string(Env &env) const {
  string s = "{ ";

  for (auto it = entries.cbegin(); it != entries.cend(); it++) {
    s += it->first->to_string(env) + " => " + it->second->to_string(env) + ", ";
  }
  if (s.size()) { s.pop_back(); s.pop_back(); }

  return s + " }" + type_string(env);
}

RecordPat::RecordPat(SymbolRef& record_name) : record_name(move(record_name)) {}
RecordPat::RecordPat(SymbolRef &record_name, vector<RecordPatField> &fields) : record_name(move(record_name)), fields(move(fields)) {}

string RecordPat::to_string(Env &env) const {
  string s = record_name.to_string(env) + " { ";

  for (auto it = fields.cbegin(); it != fields.cend(); it++) {
    s += it->first.to_string(env) + ": " + it->second->to_string(env) + ", ";
  }
  if (s.size()) { s.pop_back(); s.pop_back(); }

  return s + " }" + type_string(env);
}

SymbolPat::SymbolPat(SymbolRef &symbol) : symbol(move(symbol)) {}

string SymbolPat::to_string(Env &env) const {
  return symbol.to_string(env) + type_string(env);
}

ValuePat::ValuePat(Value value) : value(value) {}

string ValuePat::to_string(Env &env) const {
  return value.to_string(env) + type_string(env);
}

} // namespace dasl::pt
