#pragma once

#include <unordered_map>

#include "parser.hpp"

class Generator {
public:
  inline explicit Generator(NodeProg prog) : m_prog(std::move(prog)) {}

  void gen_expr(const NodeExpr& expr) {
    struct ExprVisitor {
      Generator* gen;

      void operator() (const NodeExprIntLit& expr_int_lit) const {
        gen->m_output << "    mov rax, " << expr_int_lit.int_lit.value.value() << "\n";
        gen->m_output << "    push rax\n";
      }
      void operator() (const NodeExprIdent& expr_ident) const {
        
      }
    };

    ExprVisitor visitor {.gen = this};
    std::visit(visitor, expr.var);
  }

  void gen_stmt(const NodeStmt& stmt) {
    struct StmtVisitor {
      Generator* gen;

      void operator() (const NodeStmtExit& stmt_exit) const {
        gen->gen_expr(stmt_exit.expr);

        gen->m_output << "    mov rax, 60\n";
        gen->m_output << "    pop rdi\n";
        gen->m_output << "    syscall\n";
      }
      void operator() (const NodeStmtVar& stmt_var) const {
        //if (gen->m_vars.contains(stmt_var.ident.value.value())) {
        //  std::cerr << "Error: Identifier already used: " << stmt_var.ident.value.value() << std::endl;
        //  exit(1);
        //}

        //gen->m_vars.insert({stmt_var.ident.value.value(), Var {.stack_loc = gen->m_stack_size} });
        //gen->gen_expr(stmt_var.expr);
      }
    };

    StmtVisitor visitor {.gen = this};
    std::visit(visitor, stmt.var);
  }

  [[nodiscard]] std::string generate_program() {
    m_output << "global _start\n_start:\n";

    for (const NodeStmt& stmt : m_prog.stmts) {
      gen_stmt(stmt);
    }

    return m_output.str();
  }

private:
  const NodeProg m_prog;
  std::stringstream m_output;
};
