#include "parse_tree.hxx"

using std::move;
using std::nullopt;

#include "parse_tree_expr.cxx"
#include "parse_tree_type.cxx"
#include "parse_tree_pat.cxx"
#include "parse_tree_st.cxx"
#include "parse_tree_util.cxx"

namespace dasl::pt {

PT::PT() {}

void PT::set_location(location &loc) { loc = loc; }

Program::Program(vector<unique_ptr<St>> &statements) : statements(move(statements)) {}
string Program::to_string(Env &env) const {
  string s;
  for (auto it = statements.cbegin(); it != statements.cend(); it++) {
    s += (*it)->to_string(env) + "\n";
  }
  return s;
}

}  // namespace dasl::pt
