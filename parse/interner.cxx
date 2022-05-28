#include "interner.hxx"

namespace dasl {
Interner::Interner() {}
istring Interner::get(string& s) {
  auto it = str_to_istring.find(s);
  if (it != str_to_istring.end()) {
    return it->second;
  } else {
    istring i = { istring_to_str.size() };
    str_to_istring[s] = i;
    istring_to_str.push_back(s);
    return i;
  }
}
string &Interner::get_string(istring s) {
  return istring_to_str[s.i];
}

} // namespace dasl
