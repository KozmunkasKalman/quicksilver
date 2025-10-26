#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <optional>
#include <vector>
#include <cctype>

#include "parser.hpp"

class Assembler {
public:
  inline Assembler(NodeExit root) : m_root(std::move(root)) {}

  std::string assemble() const {
    std::stringstream output;
    output << "global _start\n_start:\n";

    // [...]

    output << "    mov rax, 60\n";
    output << "    mov rdi, " << m_root.expr.int_lit.value.value() << "\n";
    output << "    syscall";

    std::fstream file("output.asm", std::ios::out);
    file << output
    file.close();

    system("nasm -felf64 output.asm");
    system("ld output.o -o output");

    exit(0);
  }

private:
  const NodeExit m_root;
};
