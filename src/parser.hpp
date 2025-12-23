#pragma once

#include <variant>

#include "lexer.hpp"
#include "arena.hpp"

struct NodeExpr;
struct NodeTermIntLit { Token int_lit; };
struct NodeTermIdent { Token ident; };
struct NodeTerm { std::variant<NodeTermIntLit*, NodeTermIdent*> var; };
struct NodeBinExprAdd { NodeExpr* ls; NodeExpr* rs; };
//struct NodeBinExprMult { NodeExpr* ls; NodeExpr* rs; };
//struct NodeBinExpr { std::variant<NodeBinExprAdd*, NodeBinExprMult*> var; };
struct NodeBinExpr { NodeBinExprAdd* add; };
struct NodeExpr { std::variant<NodeTerm*, NodeBinExpr*> var; };

struct NodeStmtExit { NodeExpr* expr; };
struct NodeStmtVar { Token ident; NodeExpr* expr; };
struct NodeStmt { std::variant<NodeStmtExit*, NodeStmtVar*> var; };

struct NodeProg { std::vector<NodeStmt*> stmts; };

class Parser {
public:
  inline explicit Parser(std::vector<Token> tokens)
    : m_tokens(std::move(tokens)), m_allocator(1024 * 1024 * 8) {} //8 mb

  std::optional<NodeTerm*> parse_term() {
    if (peek().has_value() && peek().value().type == TokenType::int_lit) {
      auto node_term_int_lit = m_allocator.alloc<NodeTermIntLit>();
      node_term_int_lit->int_lit = absorb();
      auto term = m_allocator.alloc<NodeTerm>();
      term->var = node_term_int_lit;
      return term;
    } else if (peek().has_value() && peek().value().type == TokenType::ident) {
      auto node_term_ident = m_allocator.alloc<NodeTermIdent>();
      node_term_ident->ident = absorb();
      auto term = m_allocator.alloc<NodeTerm>();
      term->var = node_term_ident;
      return term;
    } else {
      return {};
    }
  }

  std::optional<NodeExpr*> parse_expr() {
    if (auto term = parse_term()) {
      if (peek().has_value() && peek().value().type == TokenType::plus) {
        auto bin_expr = m_allocator.alloc<NodeBinExpr>();
        auto bin_expr_add = m_allocator.alloc<NodeBinExprAdd>();
        auto ls_expr = m_allocator.alloc<NodeExpr>();
        ls_expr->var = term.value();
        bin_expr_add->ls = ls_expr;
        absorb();
        if (auto rs = parse_expr()) {
          bin_expr_add->rs = rs.value();
          bin_expr->add = bin_expr_add;
          auto expr = m_allocator.alloc<NodeExpr>();
          expr->var = bin_expr;
          return expr;
        } else {
          std::cerr << "Error: Expected expression for `+` operator" << std::endl;
          exit(1);
        }
      } else {
        auto expr = m_allocator.alloc<NodeExpr>();
        expr->var = term.value();
        return expr;
      }
    } else {
      return {};
    }
  }

  std::optional<NodeStmt*> parse_stmt() {
    if ( // exit
      peek().has_value() && peek().value().type == TokenType::exit) {
      absorb();
      auto stmt_exit = m_allocator.alloc<NodeStmtExit>();
      if (auto node_expr = parse_expr()) {
        stmt_exit->expr = node_expr.value();
      } else {
        std::cerr << "Error: Invalid expression" << std::endl;
        exit(1);
      }
      auto stmt = m_allocator.alloc<NodeStmt>();
      stmt->var = stmt_exit;
      return stmt;
    } else if ( // int
      peek().has_value() && peek().value().type == TokenType::_int &&
      peek(1).has_value() && peek(1).value().type == TokenType::ident) {

      absorb();
      auto stmt_var = m_allocator.alloc<NodeStmtVar>();
      stmt_var->ident = absorb();

      if (auto expr = parse_expr()) {
        stmt_var->expr = expr.value();
      } else {
        std::cerr << "Error: Invalid expression" << std::endl;
        exit(1);
      }
      auto stmt = m_allocator.alloc<NodeStmt>();
      stmt->var = stmt_var;
      return stmt;
    } else if ( // flt
      peek().has_value() && peek().value().type == TokenType::_flt &&
      peek(1).has_value() && peek(1).value().type == TokenType::ident) {

      std::cerr << "Error: Floating point number data type class not implemented yet" << std::endl;
      exit(1);
    } else if ( // str
      peek().has_value() && peek().value().type == TokenType::_str &&
      peek(1).has_value() && peek(1).value().type == TokenType::ident) {

      std::cerr << "Error: Character string data type class not implemented yet" << std::endl;
      exit(1);
    } else if ( // null
      peek().has_value() && peek().value().type == TokenType::_null) {

      std::cerr << "Error: Null data type class not implemented yet" << std::endl;
      exit(1);
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
        std::cerr << "Error: Invalid statement" << std::endl;
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

  ArenaAlloc m_allocator;
};
