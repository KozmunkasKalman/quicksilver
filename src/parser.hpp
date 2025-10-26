#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <optional>
#include <vector>
#include <cctype>

#include "tokenizer.hpp"

struct NodeExpr { Token int_lit; };
struct NodeExit { NodeExpr expr; };

class Parser {
public:
  inline explicit Parser(std::vector<Token> tokens) : m_tokens(std::move(tokens)) {}

  std::optional<NodeExpr> parse_expr() {
    if (peek().has_value() && peek().value().type == TokenType::int_lit) {
      return NodeExpr{.int_lit = absorb()};
    } else {
      return {};
    }
  }

  std::optional<NodeExit> parse() {
    std::optional<NodeExit> exit_node;
    while (peek().has_value()) {
      if (peek().value().type == TokenType::exit) {
        absorb();
        if (auto node_expr = parse_expr()) {
          exit_node = NodeExit {.expr = node_expr.value()};

        } else {
          std::cerr << "Error: Invalid expression." << std::endl;
          exit(1);
        }
        if (peek().has_value() && peek().value().type == TokenType::newline) {
          absorb();
        } else {
          std::cerr << "Error: Invalid expression." << std::endl;
          exit(1);
        }
      }
    }
    m_index = 0;
    return exit_node;
  }

private:
  inline std::optional<Token> peek(int offset = 0) const {
    if (m_index + offset >= m_tokens.size()) {
      return std::nullopt;
    } else {
      return m_tokens.at(m_index + offset);
    }
  }

  inline Token absorb() {
    return m_tokens.at(m_index++);
  }

  const std::vector<Token> m_tokens;

  int m_index = 0;

};
