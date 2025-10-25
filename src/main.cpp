#include <iostream>
#include <fstream>
#include <sstream>
#include <optional>
#include <vector>

enum class TokenType {
  exit,
  int_lit,
  newline
};

struct Token {
  TokenType type;
  std::optional<std::string> value {};
};

std::vector<Token> tokenize(const std::string& str) {
  std::vector<Token> tokens;
  std::string b;
  for (int i = 0; i < str.length(); i++) {
    char c = str.at(i);
    if (std::isalpha(c)) {
      b.push_back(c);
      i++;
      while (std::isalnum(str.at(i))) {
        b.push_back(str.at(i));
        i++;
      }
      i--;

      if (b == "exit") {
        tokens.push_back({.type = TokenType::exit});
        b.clear();
        continue;
      } else {
        std::cerr << "Error" << std::endl;
        exit(1);
      }
    } else if (std::isdigit(c)) {
      b.push_back(c);
      i++;
      while (std::isdigit(str.at(i))) {
        b.push_back(str.at(i));
        i++;
      }
      i--;
      tokens.push_back({.type = TokenType::int_lit, .value = b});
      b.clear();
    } else if (c == 0x0A) {
      tokens.push_back({.type = TokenType::newline});
    } else if (std::isspace(c)) {
      continue;
    } else {
      std::cerr << "Error" << std::endl;
      exit(1);
    }
  }
  return tokens;
}

std::string asmify(const std::vector<Token>& tokens) {
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
  return output.str();
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

  std::vector<Token> tokens = tokenize(contents);

  std::fstream output("output.asm", std::ios::out);
  output << asmify(tokens);
  output.close();

  system("nasm -felf64 output.asm");
  system("ld output.o -o output");

  return 0;
}
