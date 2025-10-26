#include <iostream>
#include <fstream>
#include <sstream>
#include <optional>
#include <vector>
#include <cctype>

#include "tokenizer.hpp"
#include "parser.hpp"
#include "assembler.hpp"

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "Error: Incorrect usage.\nCorrect usage:\nqsc <input.qsv>" << std::endl;
    exit(1);
  }

  std::string contents;
  std::stringstream contents_stream;
  std::fstream input(argv[1], std::ios::in);
  contents_stream << input.rdbuf();
  input.close();
  contents = contents_stream.str();

  Tokenizer tokenizer(std::move(contents));
  std::vector<Token> tokens = tokenizer.tokenize();

  Parser parser(std::move(tokens));
  std::optional<NodeExit> tree = parser.parse();

  if (!tree.has_value()) {
    std::cerr << "Error: Unable to find a valid exit statement." << std::endl;
    exit(1);
  }

  Assembler assembler(std::move(tree.value()));
  assembler.assemble();

  return 0;
}
