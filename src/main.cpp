#include <iostream>
#include <fstream>
#include <sstream>
#include <optional>
#include <vector>

#include "tokenizer.hpp"

void assemble(const std::vector<Token>& tokens) {
  std::stringstream output;
  output << "global _start\n_start:\n";
  for (int i = 0; i < tokens.size(); i++) {
    const Token& token = tokens.at(i);
    if (token.type == TokenType::exit) {
      if (i + 1 < tokens.size() && tokens.at(i + 1).type == TokenType::int_lit) {
        if (i + 2 < tokens.size() && tokens.at(i + 2).type == TokenType::newline) {
          output << "    mov rax, 60\n";
          output << "    mov rdi, " << tokens.at(i + 1).value.value() << "\n";
          output << "    syscall";
        }
      }
    }
  }
  std::fstream file("output.asm", std::ios::out);
  file << output.str();
  file.close();

  system("nasm -felf64 output.asm");
  system("ld output.o -o output");
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "Error: Incorrect usage.\nCorrect usage:\nqsc <input.qsv>" << std::endl;
    return 1;
  }

  std::string contents;
  std::stringstream contents_stream;
  std::fstream input(argv[1], std::ios::in);
  contents_stream << input.rdbuf();
  input.close();
  contents = contents_stream.str();

  Tokenizer tokenizer(std::move(contents));
  std::vector<Token> tokens = tokenizer.tokenize();

  assemble(tokens);

  return 0;
}
