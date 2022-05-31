namespace dasl::pt {

Type::Type() {}

ListType::ListType() {}
string ListType::to_string(Env &env) const { return "list"; }

MapType::MapType() {}
string MapType::to_string(Env &env) const { return "map"; }

RecordType::RecordType(SymbolRef &symbol) : symbol(move(symbol)) {}
string RecordType::to_string(Env &env) const { return symbol.to_string(env); }

AnyType::AnyType() {}
string AnyType::to_string(Env &env) const { return "any"; }

PrimType::PrimType(PrimKind kind) : kind(kind) {}
string PrimType::to_string(Env &env) const {
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

} // namespace dasl::pt
