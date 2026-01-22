#include <iostream>
#include <fstream>
#include <sstream>
#include <optional>
#include <vector>
#include <cctype>
#include <variant>
#include <unordered_map>

#include "arena.hpp"



// globals
ArenaAlloc allocator(1024 * 1024 * 8);



// lexer tokens
enum class TokenType {
  // literals
  int_lit,
  flt_lit,
  str_lit,
  ident,
  // data type classes
  _bool,
  _int,
  _flt,
  _str,
  _null,
  // functional statements and calls
  load, // <input_file> - loads a files contents, usually .hg quicksilver generic headers, similar to #include in c
        //                can also be used to load files contents as an expression
  exit, // <int8>       - exits program with a mandatory exit code
  // functions
  func,              //    - keyword defines a function
  parentheses_open,  // (  - opens a functions input list
  parentheses_close, // )  - closes a functions input list
  comma,             // ,  - separates function inputs or logical statement clauses
  colon,             // :  - opens a functions or a logical statements scope
  returns,           // . - defines the type class the function returns
  gets,              // <- - passes to the function an arbitrary type class (idk why)
  period,            // .  - closes a functions scope
  // logical statements and loops
  stmt_if,        // if a < 10: "a is less than 10\n",/;
  stmt_elsif,     // elsif a !< 10 & b < 10: "while a is not, but b is less than 10\n",/;
  stmt_else,      // else: "both a and b are at least 10\n";
  stmt_while,     // while x < 10: x + 1;
  stmt_for,       // TODO: plan syntax for "for" loops
  stmt_select,    // select <input> from:
  stmt_from,      //   case = "foo": "bar\n",
  stmt_case,      //   case < 7 | case > 17: "not between 7 and 17\n",
  stmt_otherwise, //   otherwise: "none matched\n";
  semicolon,      // ; - closes logical statements and loops scopes
  // logical expressions
  _true,
  _false,
  // logical symbols
  equals,    // =
  less,      // <
  lessquals, // <=
  more,      // >
  morequals, // >=
  excl,      // !
  et,        // & "and"
  pipe,      // | "or"
  dollar,    // $ "xor"
  // math symbols
  plus,   // +
  minus,  // -
  mult,   // *
  div,    // /
  modulo, // %
  power,  // ^
  // misc symbols
  ansi_escape,    // \ - used for ansi escape sequences
  bracket_open,  // [
  bracket_close, // ]
};

struct Token {
  TokenType type;
  std::optional<std::string> value {};
};



// parser nodes
struct NodeExpr;
struct NodeTermIntLit { Token int_lit; };
struct NodeTermIdent { Token ident; };
struct NodeTerm { std::variant<NodeTermIntLit*, NodeTermIdent*> var; };
struct NodeBinExprAdd { NodeExpr* ls; NodeExpr* rs; };
struct NodeBinExprSub { NodeExpr* ls; NodeExpr* rs; };
struct NodeBinExprMult { NodeExpr* ls; NodeExpr* rs; };
struct NodeBinExprDiv { NodeExpr* ls; NodeExpr* rs; };
struct NodeBinExprMod { NodeExpr* ls; NodeExpr* rs; };
struct NodeBinExpr { std::variant<NodeBinExprAdd*, NodeBinExprSub*, NodeBinExprMult*, NodeBinExprDiv*, NodeBinExprMod*> var; };
struct NodeExpr { std::variant<NodeTerm*, NodeBinExpr*> var; };

struct NodeStmtExit { NodeExpr* expr; };
struct NodeStmtVar { Token ident; NodeExpr* expr; };
struct NodeStmt { std::variant<NodeStmtExit*, NodeStmtVar*> var; };

struct NodeProg { std::vector<NodeStmt> stmts; };



// code generation
size_t stack_size = 0;
struct Var { size_t stack_loc; };
std::unordered_map<std::string, Var> vars {};



// helper functions
std::optional<char> peek_char(std::string& source, int index, int offset = 0) {
  if (index + offset >= source.length()) return std::nullopt;
  return source.at(index + offset);
}
char consume_char(std::string& source, int& index) {
  return source.at(index++);
}

std::optional<Token> peek_token(std::vector<Token> tokens, int index, int offset = 0) {
  if (index + offset >= tokens.size()) return std::nullopt;
  return tokens[index + offset];
}
Token consume_token(std::vector<Token> tokens, int& index) {
  return tokens[index++];
}

std::string push(const std::string& reg) {
  stack_size++;
  std::string output = "    push " + reg + "\n";
  return output;
}
std::string pop(const std::string& reg) {
  stack_size--;
  std::string output = "    pop " + reg + "\n";
  return output;
}

int get_precedence(TokenType type) {
  switch (type) {
    case TokenType::plus:
    case TokenType::minus:
      return 1;
    case TokenType::mult:
    case TokenType::div:
    case TokenType::modulo:
      return 2;
    case TokenType::power:
      return 3; // right-associative
    default: return -1;
  }
}
bool is_right_associative(TokenType type) {
  return type == TokenType::power;
}




std::string read_file(char* input_file) {
  std::string contents;
  std::stringstream contents_stream;
  std::fstream input(input_file, std::ios::in);
  contents_stream << input.rdbuf();
  contents = contents_stream.str();
  input.close();

  return contents;
}



std::vector<Token> tokenize(std::string source) {
  std::vector<Token> tokens;
  std::string b; // buffer
  int index = 0;

  while (peek_char(source, index).has_value()) {
    // first check if its a whitespace, if so then ignore it
    if (std::isspace(peek_char(source, index).value())) { // space
      consume_char(source, index);
    } else if (peek_char(source, index).value() == 0x0A) { // end of line
      consume_char(source, index);
    // then check for keywords
    } else if (std::isalpha(peek_char(source, index).value())) {
      b.push_back(consume_char(source, index));
      while (peek_char(source, index).has_value() && std::isalnum(peek_char(source, index).value())) {
        b.push_back(consume_char(source, index));
      }
      // data type classes
      // TODO: make types semi-direct: "null0", "bool1", "int(2^(3-6))", "flt64", "chr8", "str(2^(4-6))"
      if (b == "bool") { // boolean
        tokens.push_back( { .type = TokenType::_bool } );
        b.clear();
      } else if (b == "int") { // integer
        tokens.push_back( { .type = TokenType::_int } );
        b.clear();
      } else if (b == "flt") { // floating point number
        tokens.push_back( { .type = TokenType::_flt } );
        b.clear();
      } else if (b == "str") { // character string
        tokens.push_back( { .type = TokenType::_str } );
        b.clear();
      } else if (b == "null") { // null (bottom type)
        tokens.push_back( { .type = TokenType::_null } );
        b.clear();
      // functions, statements, calls
      } else if (b == "func") { // function
        tokens.push_back( { .type = TokenType::func } );
        b.clear();
      } else if (b == "load") { // load file
        tokens.push_back( { .type = TokenType::load } );
        b.clear();
      } else if (b == "exit") { // exit call
        tokens.push_back( { .type = TokenType::exit } );
        b.clear();
      } else if (b == "if") {
        tokens.push_back( { .type = TokenType::stmt_if } );
        b.clear();
      } else if (b == "elsif") {
        tokens.push_back( { .type = TokenType::stmt_elsif } );
        b.clear();
      } else if (b == "else") {
        tokens.push_back( { .type = TokenType::stmt_else } );
        b.clear();
      } else if (b == "while") {
        tokens.push_back( { .type = TokenType::stmt_while } );
        b.clear();
      } else if (b == "for") {
        tokens.push_back( { .type = TokenType::stmt_for } );
        b.clear();
      } else if (b == "select") {
        tokens.push_back( { .type = TokenType::stmt_select } );
        b.clear();
      } else if (b == "from") {
        tokens.push_back( { .type = TokenType::stmt_from } );
        b.clear();
      } else if (b == "case") {
        tokens.push_back( { .type = TokenType::stmt_case } );
        b.clear();
      } else if (b == "otherwise") {
        tokens.push_back( { .type = TokenType::stmt_otherwise } );
        b.clear();
      } else if (b == "true") {
        tokens.push_back( { .type = TokenType::_true } );
        b.clear();
      } else if (b == "false") {
        tokens.push_back( { .type = TokenType::_false } );
        b.clear();
      } else { // identifier
        tokens.push_back( { .type = TokenType::ident, .value = b } );
        b.clear();
      }
    // then check for character strings
    } else if (peek_char(source, index).value() == '"') { // string
      consume_char(source, index);
      while (peek_char(source, index).has_value() && peek_char(source, index).value() != '"') {
        if (peek_char(source, index).value() != EOF) {
          b.push_back(consume_char(source, index));
        } else {
          std::cerr << "Error: String was never closed off" << std::endl;
          exit(1);
        }
        tokens.push_back( { .type = TokenType::str_lit, .value = b } );
        b.clear();
      }
      consume_char(source, index);
      tokens.push_back( { .type = TokenType::str_lit, .value = b } );
      b.clear();
      continue;
    // then check for numbers
    } else if (std::isdigit(peek_char(source, index).value())) { // number
      b.push_back(consume_char(source, index));
      while (peek_char(source, index).has_value() && std::isdigit(peek_char(source, index).value())) {
        b.push_back(consume_char(source, index));
      }
      tokens.push_back( { .type = TokenType::int_lit, .value = b } );
      b.clear();
    // then check for single character symbols
    } else if (peek_char(source, index).value() == '(') {
      consume_char(source, index);
      tokens.push_back( { .type = TokenType::parentheses_open } );
    } else if (peek_char(source, index).value() == ')') {
      consume_char(source, index);
      tokens.push_back( { .type = TokenType::parentheses_close } );
    } else if (peek_char(source, index).value() == ',') {
      consume_char(source, index);
      tokens.push_back( { .type = TokenType::comma } );
    } else if (peek_char(source, index).value() == ':') {
      consume_char(source, index);
      tokens.push_back( { .type = TokenType::colon } );
    } else if (peek_char(source, index).value() == ';') {
      consume_char(source, index);
      tokens.push_back( { .type = TokenType::semicolon } );
    } else if (peek_char(source, index).value() == '.') {
      consume_char(source, index);
      tokens.push_back( { .type = TokenType::period } );
    } else if (peek_char(source, index).value() == '+') {
      consume_char(source, index);
      tokens.push_back( { .type = TokenType::plus } );
    } else if (peek_char(source, index).value() == '-') {
      consume_char(source, index);
      tokens.push_back( { .type = TokenType::minus } );
    // TODO: implement multiplication by juxtaposition of two expressions properly, for now its * since prototype implementation of it cause it to happen even upon variable declaration and it multiplied itself indefinitely
    } else if (peek_char(source, index).value() == '*') {
      consume_char(source, index);
      tokens.push_back( { .type = TokenType::mult } );
    } else if (peek_char(source, index).value() == '/') {
      consume_char(source, index);
      tokens.push_back( { .type = TokenType::div } );
    } else if (peek_char(source, index).value() == '%') {
      consume_char(source, index);
      tokens.push_back( { .type = TokenType::modulo } );
    } else if (peek_char(source, index).value() == '^') {
      consume_char(source, index);
      tokens.push_back( { .type = TokenType::power } );
    } else if (peek_char(source, index).value() == '=') {
      consume_char(source, index);
      tokens.push_back( { .type = TokenType::equals } );
    } else if (peek_char(source, index).value() == '<') {
      consume_char(source, index);
      tokens.push_back( { .type = TokenType::less } );
    } else if (peek_char(source, index).value() == '>') {
      consume_char(source, index);
      tokens.push_back( { .type = TokenType::more } );
    } else if (peek_char(source, index).value() == '!') {
      consume_char(source, index);
      tokens.push_back( { .type = TokenType::excl } );
    } else if (peek_char(source, index).value() == '&') {
      consume_char(source, index);
      tokens.push_back( { .type = TokenType::et } );
    } else if (peek_char(source, index).value() == '|') {
      consume_char(source, index);
      tokens.push_back( { .type = TokenType::pipe } );
    } else if (peek_char(source, index).value() == '$') {
      consume_char(source, index);
      tokens.push_back( { .type = TokenType::dollar } );
    } else if (peek_char(source, index).value() == '[') {
      consume_char(source, index);
      tokens.push_back( { .type = TokenType::bracket_open } );
    } else if (peek_char(source, index).value() == ']') {
      consume_char(source, index);
      tokens.push_back( { .type = TokenType::bracket_close } );
    } else if (peek_char(source, index).value() == 0x5C) { // '\'
      consume_char(source, index);
      tokens.push_back( { .type = TokenType::ansi_escape } );
    } else if (peek_char(source, index).value() == '#') { // comment lasts until another #, newline, or end of file
      consume_char(source, index);
      while (peek_char(source, index).has_value() && peek_char(source, index).value() != '#' && peek_char(source, index).value() != '\n')
        consume_char(source, index);
      if (peek_char(source, index).has_value() && peek_char(source, index).value() == '#') consume_char(source, index);
    // multi character symbols
    //} else if (std::ispunct(peek_char(source, index).value())) { // symbol(s)
    //  while (peek_char(source, index).has_value() && std::isgraph(peek_char(source, index).value())) {
    //    b.push_back(consume_char(source, index));
    //  }
    //  if (b == "<=") {
    //    tokens.push_back( { .type = TokenType::lessquals } );
    //    b.clear();
    //  } else if (b == ">=") {
    //    tokens.push_back( { .type = TokenType::morequals } );
    //    b.clear();
    //  } else if (b == "->") {
    //    tokens.push_back( { .type = TokenType::returns } );
    //    b.clear();
    //  } else if (b == "<-") {
    //    tokens.push_back( { .type = TokenType::gets } );
    //    b.clear();
    //  }
    // if neither matched, throw an error
    } else {
      std::cerr << "Error: Unable to handle character at " << index << std::endl;
      exit(1);
    }
  }
  return tokens;
}



std::optional<NodeTerm*> parse_term(std::vector<Token>& tokens, int& index) {
  if (peek_token(tokens, index).has_value() && peek_token(tokens, index).value().type == TokenType::int_lit) {
    auto node_term_int_lit = allocator.alloc<NodeTermIntLit>();
    node_term_int_lit->int_lit = consume_token(tokens, index);
    auto term = allocator.alloc<NodeTerm>();
    term->var = node_term_int_lit;
    return term;
  } else if (peek_token(tokens, index).has_value() && peek_token(tokens, index).value().type == TokenType::ident) {
    auto node_term_ident = allocator.alloc<NodeTermIdent>();
    node_term_ident->ident = consume_token(tokens, index);
    auto term = allocator.alloc<NodeTerm>();
    term->var = node_term_ident;
    return term;
  } else {
    return {};
  }
}
std::optional<NodeExpr*> parse_bin_expr(std::vector<Token>& tokens, int& index, int min_prec = 0) {
  auto lhs_term = parse_term(tokens, index);
  if (!lhs_term) return {};

  auto lhs_expr = allocator.alloc<NodeExpr>();
  lhs_expr->var = lhs_term.value();

  while (peek_token(tokens, index).has_value()) {
    TokenType tok_type = peek_token(tokens, index).value().type;
    int prec = get_precedence(tok_type);

    if (prec < min_prec) break;

    consume_token(tokens, index);
    TokenType op = tok_type;

    int next_min_prec = prec;
    if (!is_right_associative(op)) next_min_prec += 1;

    auto rhs_expr = parse_bin_expr(tokens, index, next_min_prec);
    if (!rhs_expr) {
        std::cerr << "Error: Expected expression after operator\n";
        exit(1);
    }

    auto bin_expr = allocator.alloc<NodeBinExpr>();
    if (op == TokenType::plus) {
      auto node = allocator.alloc<NodeBinExprAdd>();
      node->ls = lhs_expr;
      node->rs = rhs_expr.value();
      bin_expr->var = node;
    } else if (op == TokenType::minus) {
      auto node = allocator.alloc<NodeBinExprSub>();
      node->ls = lhs_expr;
      node->rs = rhs_expr.value();
      bin_expr->var = node;
    } else if (op == TokenType::mult) {
      auto node = allocator.alloc<NodeBinExprMult>();
      node->ls = lhs_expr;
      node->rs = rhs_expr.value();
      bin_expr->var = node;
    } else if (op == TokenType::div) {
      auto node = allocator.alloc<NodeBinExprDiv>();
      node->ls = lhs_expr;
      node->rs = rhs_expr.value();
      bin_expr->var = node;
    } else if (op == TokenType::modulo) {
      auto node = allocator.alloc<NodeBinExprMod>();
      node->ls = lhs_expr;
      node->rs = rhs_expr.value();
      bin_expr->var = node;
    } else if (op == TokenType::power) {
      std::cerr << "Error: Power operator not implemented yet\n";
      exit(1);
    }

    lhs_expr = allocator.alloc<NodeExpr>();
    lhs_expr->var = bin_expr;
  }

  return lhs_expr;
}
std::optional<NodeExpr*> parse_expr(std::vector<Token>& tokens, int& index) {
  return parse_bin_expr(tokens, index, 0);
  // if (auto term = parse_term(tokens, index)) {
  //   if (peek_token(tokens, index).has_value() &&
  //       (peek_token(tokens, index).value().type == TokenType::plus ||
  //        peek_token(tokens, index).value().type == TokenType::minus ||
  //        peek_token(tokens, index).value().type == TokenType::mult ||
  //        peek_token(tokens, index).value().type == TokenType::div ||
  //        peek_token(tokens, index).value().type == TokenType::power) ) {
  //     return parse_bin_expr(tokens, index);
  //   } else {
  //     auto expr = allocator.alloc<NodeExpr>();
  //     expr->var = term.value();
  //     return expr;
  //   }
  // } else {
  //   return {};
  // }
}
std::optional<NodeStmt*> parse_stmt(std::vector<Token>& tokens, int& index) {
  auto stmt = allocator.alloc<NodeStmt>();
  if (peek_token(tokens, index).has_value() && peek_token(tokens, index).value().type == TokenType::exit) {
    consume_token(tokens, index);
    auto stmt_exit = allocator.alloc<NodeStmtExit>();
    if (auto node_expr = parse_expr(tokens, index)) {
      stmt_exit->expr = node_expr.value();
    } else {
      std::cerr << "Error: Invalid expression" << std::endl;
      exit(1);
    }
    stmt->var = stmt_exit;
  } else if (peek_token(tokens, index).has_value() && peek_token(tokens, index).value().type == TokenType::_int && peek_token(tokens, index, 1).has_value() && peek_token(tokens, index, 1).value().type == TokenType::ident) {
    consume_token(tokens, index);
    auto stmt_var = allocator.alloc<NodeStmtVar>();
    stmt_var->ident = consume_token(tokens, index);

    if (auto expr = parse_expr(tokens, index)) {
      stmt_var->expr = expr.value();
    } else {
      std::cerr << "Error: Invalid expression" << std::endl;
      exit(1);
    }
    stmt->var = stmt_var;
  } else if (peek_token(tokens, index).has_value() && peek_token(tokens, index).value().type == TokenType::_flt && peek_token(tokens, index, 1).has_value() && peek_token(tokens, index, 1).value().type == TokenType::ident) {
    std::cerr << "Error: Floating point number data type class not implemented yet" << std::endl;
    exit(1);
  } else if (peek_token(tokens, index).has_value() && peek_token(tokens, index).value().type == TokenType::_str && peek_token(tokens, index, 1).has_value() && peek_token(tokens, index, 1).value().type == TokenType::ident) {
    std::cerr << "Error: Character string data type class not implemented yet" << std::endl;
    exit(1);
  } else if ( // null
    peek_token(tokens, index).has_value() && peek_token(tokens, index).value().type == TokenType::_null) {

    std::cerr << "Error: Null data type class not implemented yet" << std::endl;
    exit(1);
  } else { stmt = {}; }

  return stmt;
}
std::optional<NodeProg> parse_program(std::vector<Token> tokens) {
  NodeProg prog;
  int index = 0;

  while (peek_token(tokens, index).has_value()) {
    if (auto stmt = parse_stmt(tokens, index)) {
      prog.stmts.push_back(*stmt.value());
    } else {
      std::cerr << "Error: Invalid statement" << std::endl;
      exit(1);
    }
  }
  
  return prog;
}



void gen_term(const NodeTerm* term, std::stringstream& output) {
  if (std::holds_alternative<NodeTermIntLit*>(term->var)) {
    auto* lit = std::get<NodeTermIntLit*>(term->var);
    output << "    mov r10, " << lit->int_lit.value.value() << "\n";
    output << push("r10");
  } else if (std::holds_alternative<NodeTermIdent*>(term->var)) {
    auto* ident = std::get<NodeTermIdent*>(term->var);

    if (!vars.contains(ident->ident.value.value())) {
      std::cerr << "Error: Undeclared identifier: " << ident->ident.value.value() << std::endl;
      exit(1);
    }

    const auto& var = vars.at(ident->ident.value.value());
    size_t offset = (stack_size - var.stack_loc - 1) * 8;

    output << "    mov r10, QWORD [rsp + " << offset << "]\n";
    output << push("r10");
  };
}
void gen_expr(const NodeExpr* expr, std::stringstream& output);
void gen_bin_expr(const NodeBinExpr* bin_expr, std::stringstream& output) {
  if (std::holds_alternative<NodeBinExprAdd*>(bin_expr->var)) {
    auto* add = std::get<NodeBinExprAdd*>(bin_expr->var);

    gen_expr(add->ls, output);
    gen_expr(add->rs, output);

    output << pop("r10");
    output << pop("r11");
    output << "    add r10, r11\n";
    output << push("r10");
  } else if (std::holds_alternative<NodeBinExprSub*>(bin_expr->var)) {
    auto* sub = std::get<NodeBinExprSub*>(bin_expr->var);

    gen_expr(sub->ls, output);
    gen_expr(sub->rs, output);

    output << pop("r11");
    output << pop("r10");
    output << "    sub r10, r11\n";
    output << push("r10");
  } else if (std::holds_alternative<NodeBinExprMult*>(bin_expr->var)) {
    auto* mult = std::get<NodeBinExprMult*>(bin_expr->var);

    gen_expr(mult->ls, output);
    gen_expr(mult->rs, output);

    output << pop("r10");
    output << pop("r11");
    output << "    mul r10, r11\n";
    output << push("r10");
  } else if (std::holds_alternative<NodeBinExprDiv*>(bin_expr->var)) {
    auto* div = std::get<NodeBinExprDiv*>(bin_expr->var);

    gen_expr(div->ls, output);
    gen_expr(div->rs, output);

    output << pop("r11");
    output << pop("r10");
    output << "    mov rax, r10\n";
    output << "    xor rdx, rdx\n";
    output << "    div r11\n";
    output << push("rax");
  } else if (std::holds_alternative<NodeBinExprMod*>(bin_expr->var)) {
    auto* modulo = std::get<NodeBinExprMod*>(bin_expr->var);

    gen_expr(modulo->ls, output);
    gen_expr(modulo->rs, output);

    output << pop("r11");
    output << pop("r10");
    output << "    mov rax, r10\n";
    output << "    xor rdx, rdx\n";
    output << "    div r11\n";
    output << push("rdx");
  }
}
void gen_expr(const NodeExpr* expr, std::stringstream& output) {
  if (std::holds_alternative<NodeTerm*>(expr->var)) {
    gen_term(std::get<NodeTerm*>(expr->var), output);
  }
  else if (std::holds_alternative<NodeBinExpr*>(expr->var)) {
    gen_bin_expr(std::get<NodeBinExpr*>(expr->var), output);
  }
}
void gen_stmt(const NodeStmt* stmt, std::stringstream& output) {
  if (std::holds_alternative<NodeStmtExit*>(stmt->var)) {
    auto* s = std::get<NodeStmtExit*>(stmt->var);

    gen_expr(s->expr, output);
    output << "    mov rax, 60\n";
    output << pop("rdi");
    output << "    syscall\n";
  }
  else if (std::holds_alternative<NodeStmtVar*>(stmt->var)) {
    auto* s = std::get<NodeStmtVar*>(stmt->var);

    if (vars.contains(s->ident.value.value())) {
      std::cerr << "Error: Identifier already used: " << s->ident.value.value() << std::endl;
      exit(1);
    }

    vars.insert({ s->ident.value.value(), Var{ stack_size } });
    gen_expr(s->expr, output);
  }
}
std::string generate_program(NodeProg program) {
  std::stringstream output;
  output << "global _start\n_start:\n";

  for (const NodeStmt stmt : program.stmts) {
    gen_stmt(&stmt, output);
  }

  output << "    ret\n";

  return output.str();
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

  std::vector<Token> tokens = tokenize(contents);

  // for debugging tokens
  // for (int i = 0; i < tokens.size(); i++) {
  //   std::cout << static_cast<int>(tokens[i].type) << std::endl;
  // }

  std::optional<NodeProg> program = parse_program(tokens);
  
  if (!program.has_value()) {
    std::cerr << "Error: Invalid program." << std::endl;
    exit(1);
  }

  std::string code = generate_program(program.value());

  if (argc == 3) {
    assemble(code, argv[2]);
  } else {
    assemble(code);
  }

  return 0;
}
