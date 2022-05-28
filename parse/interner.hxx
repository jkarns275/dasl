#ifndef INTERNER_HXX
#define INTERNER_HXX

#include <map>
#include <vector>
#include <string>
using std::string;

namespace dasl {

struct istring { size_t i; };

class Interner {
  std::map<string, istring> str_to_istring;
  std::vector<string> istring_to_str;

 public:
  Interner();

  istring get(string& s);
  string &get_string(istring s);
};

} // namespace dasl

#endif // INTERNER_HXX
