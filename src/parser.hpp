#pragma once

#include <variant>

#include "tokenizer.hpp"

struct NodeExprIntLit { Token int_lit; };
struct NodeExprIdent { Token ident; };
struct NodeExpr { std::variant<NodeExprIntLit, NodeExprIdent> var; };

struct NodeStmtExit { NodeExpr expr; };
struct NodeStmtVar { Token ident; NodeExpr expr; };
struct NodeStmt { std::variant<NodeStmtExit, NodeStmtVar> var; };

struct NodeProg { std::vector<NodeStmt> stmts; };

class Parser {
public:
  inline explicit Parser(std::vector<Token> tokens) : m_tokens(std::move(tokens)) {}

  std::optional<NodeExpr> parse_expr() {
    if (peek().has_value() && peek().value().type == TokenType::int_lit) {
      return NodeExpr {.var = NodeExprIntLit {.int_lit = absorb()} };
    } else if (peek().has_value() && peek().value().type == TokenType::ident) {
      return NodeExpr {.var = NodeExprIdent {.ident = absorb()}};
    } else {
      return {};
    }
  }

  std::optional<NodeStmt> parse_stmt() {
    if (peek().has_value() && peek().value().type == TokenType::exit) {
      absorb();
      NodeStmtExit stmt_exit;
      if (auto node_expr = parse_expr()) {
        stmt_exit = {.expr = node_expr.value()};
      } else {
        std::cerr << "Error: Invalid expression." << std::endl;
        exit(1);
      }
      return NodeStmt {.var = stmt_exit};
    } else if (
      peek().has_value() && peek().value().type == TokenType::var &&
      peek(1).has_value() && peek(1).value().type == TokenType::ident
      ) {

      absorb();
      auto stmt_var = NodeStmtVar {.ident = absorb()};
      if (auto expr = parse_expr()) {
        stmt_var.expr = expr.value();
      } else {
        std::cerr << "Error: Invalid expression." << std::endl;
        exit(1);
      }
      return NodeStmt {.var = stmt_var};
    } else {
      return {};
    }
  }

  std::optional<NodeProg> parse_prog() {
    NodeProg prog;

    while (peek().has_value()) {
      if (auto stmt = parse_stmt()) {
        prog.stmts.push_back(stmt.value());
      } else {
        std::cerr << "Error: Invalid statement." << std::endl;
        exit(1);
      }
    }

    return prog;
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
