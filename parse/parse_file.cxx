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

string parse(string program) {
  std::istringstream input(program);
  dasl::Lexer lexer;
  lexer.switch_streams(input, cout);
  Env env;
  dasl::Parser parser(lexer, env);
  int res = parser.parse();
  if (res) {
    std::cout << "FAILED TO PARSE!" << std::endl;
    exit(1);
  }
  return env.pt->to_string(env);
}

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cout << "ERROR: You must supply a path to program to lex!";
    return 1;
  }
  std::string path = argv[1];
  vector<char> prog = read_file(path);
  string p = parse(string(prog.begin(), prog.end()));
  std::cout << p;
}
