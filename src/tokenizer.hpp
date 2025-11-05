#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <optional>
#include <vector>
#include <cctype>

enum class TokenType {
  exit,
  int_lit,
  ident,
  var,
  equals,
  parentheses_open,
  parentheses_close,
  colon,
  period,
  newline
};

struct Token {
  TokenType type;
  std::optional<std::string> value {};
};

class Tokenizer {
public:
  inline explicit Tokenizer(std::string source) : m_source(std::move(source)) { }

  inline std::vector<Token> tokenize() {
    std::vector<Token> tokens;
    std::string b; // buffer

    while (peek().has_value()) {
      if (std::isalpha(peek().value())) { // is letters
        b.push_back(absorb());
        while (peek().has_value() && std::isalnum(peek().value())) {
          b.push_back(absorb());
        }
        if (b == "exit") {
          tokens.push_back({.type = TokenType::exit});
          b.clear();
          continue;
        } else if (b == "var"){
          tokens.push_back({.type = TokenType::var});
          b.clear();
          continue;
        } else {
          tokens.push_back({.type = TokenType::ident, .value = b});
          b.clear();
          continue;
        }
      } else if (std::isdigit(peek().value())) { // is number
        b.push_back(absorb());
        while (peek().has_value() && std::isdigit(peek().value())) {
          b.push_back(absorb());
        }
        tokens.push_back({.type = TokenType::int_lit, .value = b});
        b.clear();
        continue;
      } else if (peek().value() == '=') {
        absorb();
        tokens.push_back({.type = TokenType::equals});
        continue;
      } else if (peek().value() == '(') {
        absorb();
        tokens.push_back({.type = TokenType::parentheses_open});
        continue;
      } else if (peek().value() == ')') {
        absorb();
        tokens.push_back({.type = TokenType::parentheses_close});
        continue;
      } else if (peek().value() == ':') {
        absorb();
        tokens.push_back({.type = TokenType::colon});
        continue;
      } else if (peek().value() == '.') {
        absorb();
        tokens.push_back({.type = TokenType::period});
        continue;
      } else if (peek().value() == 0x0A) { // is line end
        absorb();
        tokens.push_back({.type = TokenType::newline});
        continue;
      } else if (std::isspace(peek().value())) { // is space
        m_index++;
        continue;
      } else {
        std::cerr << "Error: Unable to handle character at " << m_index << std::endl;
        exit(1);
      }
    }
    m_index = 0;
    return tokens;
  }

private:
  inline std::optional<char> peek(int offset = 0) const {
    if (m_index + offset >= m_source.length()) {
      return std::nullopt;
    } else {
      return m_source.at(m_index + offset);
    }
  }

  inline char absorb() {
    return m_source.at(m_index++);
  }

  const std::string m_source;

  int m_index = 0;
};
