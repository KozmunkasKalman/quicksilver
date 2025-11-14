#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <optional>
#include <vector>
#include <cctype>

enum class TokenType {
  int_lit,
  ident,
  //keywords
  func,
  exit,
  //variable types
  _int,
  _flt,
  _str,
  _null,
  //symbols
  equals,
  plus,
  minus,
  et,
  comma,
  semicolon,
  parentheses_open,
  parentheses_close,
  colon,
  period
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
        // why the fuck do switch statements not accept strings even fucking bash does that... TODO: add switch statements to qs that support strings as input
        if (b == "func") {
          tokens.push_back( { .type = TokenType::func } );
          b.clear();
          continue;
        } else if (b == "exit") {
          tokens.push_back( { .type = TokenType::exit } );
          b.clear();
          continue;
        } else if (b == "int"){
          tokens.push_back( { .type = TokenType::_int } );
          b.clear();
          continue;
        } else if (b == "flt"){
          tokens.push_back( { .type = TokenType::_flt } );
          b.clear();
          continue;
        } else if (b == "str"){
          tokens.push_back( { .type = TokenType::_str } );
          b.clear();
          continue;
        } else if (b == "null"){
          tokens.push_back( { .type = TokenType::_null } );
          b.clear();
          continue;
        } else { //identifier
          tokens.push_back( { .type = TokenType::ident, .value = b } );
          b.clear();
          continue;
        }
      } else if (std::isdigit(peek().value())) { // is number
        b.push_back(absorb());
        while (peek().has_value() && std::isdigit(peek().value())) {
          b.push_back(absorb());
        }
        tokens.push_back( { .type = TokenType::int_lit, .value = b } );
        b.clear();
        continue;
      } else if (peek().value() == '+') {
        absorb();
        tokens.push_back( { .type = TokenType::plus } );
        continue;
      } else if (peek().value() == '-') {
        absorb();
        tokens.push_back( { .type = TokenType::minus } );
        continue;
      } else if (peek().value() == '=') {
        absorb();
        tokens.push_back( { .type = TokenType::equals } );
        continue;
      } else if (peek().value() == '&') {
        absorb();
        tokens.push_back( { .type = TokenType::et } );
        continue;
      } else if (peek().value() == '(') {
        absorb();
        tokens.push_back( { .type = TokenType::parentheses_open } );
        continue;
      } else if (peek().value() == ')') {
        absorb();
        tokens.push_back( { .type = TokenType::parentheses_close } );
        continue;
      } else if (peek().value() == ':') {
        absorb();
        tokens.push_back( { .type = TokenType::colon } );
        continue;
      } else if (peek().value() == '.') {
        absorb();
        tokens.push_back( { .type = TokenType::period } );
        continue;
      } else if (peek().value() == 0x0A) { // is line end
        absorb();
        //tokens.push_back( { .type = TokenType::newline } );
        continue;
      } else if (std::isspace(peek().value())) { // is space
        absorb();
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
