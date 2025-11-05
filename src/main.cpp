#include <iostream>
#include <fstream>
#include <sstream>
#include <optional>
#include <vector>

#include "tokenizer.hpp"
#include "parser.hpp"
#include "generator.hpp"

std::string read_file(char* input_file) {
  std::string contents;
  std::stringstream contents_stream;
  std::fstream input(input_file, std::ios::in);
  contents_stream << input.rdbuf();
  contents = contents_stream.str();
  input.close();

  return contents;
}

void assemble(std::string code) {
  std::fstream file("output.asm", std::ios::out);
  file << code;
  file.close();

  system("nasm -felf64 output.asm");
  system("ld output.o -o output");
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "Error: Incorrect usage.\nCorrect usage:\nqsc <input.qsv>" << std::endl;
    exit(1);
  }

  std::string contents = read_file(argv[1]);

  Tokenizer tokenizer(std::move(contents));
  std::vector<Token> tokens = tokenizer.tokenize();

  Parser parser(std::move(tokens));
  std::optional<NodeProg> program = parser.parse_prog();

  if (!program.has_value()) {
    std::cerr << "Error: Invalid program." << std::endl;
    exit(1);
  }

  Generator generator(std::move(program.value()));
  std::string output = generator.generate_program();

  assemble(output);

  return 0;
}
