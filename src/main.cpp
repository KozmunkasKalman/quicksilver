#include <iostream>
#include <fstream>
#include <sstream>
#include <optional>
#include <vector>

#include "lexer.hpp"
#include "parser.hpp"
#include "generator.hpp"
#include "arena.hpp"

std::string read_file(char* input_file) {
  std::string contents;
  std::stringstream contents_stream;
  std::fstream input(input_file, std::ios::in);
  contents_stream << input.rdbuf();
  contents = contents_stream.str();
  input.close();

  return contents;
}

void assemble(std::string code, const std::string& output = "output") {
  std::fstream file(output + ".asm", std::ios::out);
  file << code;
  file.close();

  std::string nasm_cmd = "nasm -felf64 " + output + ".asm";
  std::string ld_cmd = "ld " + output + ".o -o " + output;

  system(nasm_cmd.c_str());
  system(ld_cmd.c_str());
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    // TODO: implement flags such as -o for output, and -k to keep the .asm and .o files
    std::cerr << "Error: Incorrect usage: No input file\nCorrect usage:\n  qsc <input.qsv> [<output>]" << std::endl;
    exit(1);
  }

  std::string contents = read_file(argv[1]);

  Tokenizer tokenizer(std::move(contents));
  std::vector<Token> tokens = tokenizer.tokenize();
  // nstead of std::move, just doing it like this would most likely be more efficient:
  // Tokenizer tokenizer;
  // std::vector<Token> tokens = tokenizer.tokenize(contents);

  Parser parser(std::move(tokens));
  std::optional<NodeProg> program = parser.parse_prog();

  if (!program.has_value()) {
    std::cerr << "Error: Invalid program." << std::endl;
    exit(1);
  }

  Generator generator(std::move(program.value()));
  std::string code = generator.generate_program();

  if (argc == 3) {
    assemble(code, argv[2]);
  } else {
    assemble(code);
  }

  return 0;
}
