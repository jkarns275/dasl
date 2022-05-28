#include <string>
using std::string;
#include <sstream>
#include <fstream>
#include <iostream>
#include <filesystem>
namespace fs = std::filesystem;

#include <vector>
using std::vector;

#include "lexer.hxx"
#include <parser.hxx>

vector<char> read_file(std::string p) {
  std::ifstream file(p, std::ios::binary | std::ios::ate);
  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);

  std::vector<char> buffer(size);
  if (file.read(buffer.data(), size)) {
    return buffer;
  }

  std::cout << "Failed to read file " << p << std::endl;
  exit(1);
}

vector<string> split_by_lines(string s) {
  std::stringstream ss(s);
  std::string to;

  vector<string> lines;
  while (std::getline(ss, to, '\n'))
    lines.push_back(to);
  return lines;
}

vector<string> lex(string program) {
  std::istringstream input(program);
  dasl::Lexer lexer;
  lexer.switch_streams(input, cout);
  vector<string> lexemes;
  while (1) {
    auto tok = lexer.get_next_token();
    if (tok.kind() != dasl::Parser::token::TOKEN_END) {
      lexemes.push_back(tok.name());
    } else {
      break;
    }
  }
  return lexemes;
}

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cout << "ERROR: You must supply a path to program to lex!";
    return 1;
  }
  std::string path = argv[1];
    vector<char> prog = read_file(path);
    vector<string> actual_lexemes = lex(string(prog.begin(), prog.end()));
    for (int i = 0; i < actual_lexemes.size(); i++)
      std::cout << actual_lexemes[i] << std::endl;
}
