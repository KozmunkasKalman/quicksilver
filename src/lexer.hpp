#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <optional>
#include <vector>
#include <cctype>

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
  load, // <input_file> - loads a file, usually .hg quicksilver generic headers
  exit, // <int8>       - exits program with a mandatory exit code
  // functions
  func,              //    - keyword defines a function
  parentheses_open,  // (  - opens a functions input list
  parentheses_close, // )  - closes a functions input list
  comma,             // ,  - separates function inputs or logical statement clauses
  colon,             // :  - opens a functions or a logical statements scope
  returns,           // -> - defines the type class the function returns
  gets,              // <- - passes to the function an arbitrary type class (idk why)
  period,            // .  - closes a functions scope
  // logical statements and loops
  stmt_if,        // if a < 10: "a is less than 10\n",/;
  stmt_elsif,     // elsif a !< 10 & b < 10: "while a is not, but b is less than 10\n",/;
  stmt_else,      // else: "both a and b are at least 10\n";
  stmt_while,     // while x < 10: x + 1;
                  // TODO: plan syntax for "for" loops
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
  et,        // &  "and"
  pipe,      // |  "or"
  dollar,    // $  "xor"
  // math symbols
  plus,  // +
  minus, // -
  mult,  // *
  div,   // /
  power, // ^
  // misc symbols
  escape,    // \ - used for ansi escape sequences
  box_open,  // [ - opens a box
  box_close, // ] - closes a box
  octothorpe // # - marks text until end of line as comment
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
      char c = peek().value();
      if (std::isalpha(c)) { // letters
        b.push_back(absorb());
        while (peek().has_value() && std::isalnum(c)) {
          b.push_back(absorb());
        }
        // why the fuck do switch statements not accept strings even fucking bash does that
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
        // TODO: "for" loops
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
      } else if (c == '"') { // string
        absorb();
        while (peek().has_value() && c != '"') {
          if (c != EOF) {
            b.push_back(absorb());
          } else {
            std::cerr << "Error: String was never closed off" << std::endl;
            exit(1);
          }
        }
        absorb();
        tokens.push_back( { .type = TokenType::str_lit, .value = b } );
        b.clear();
        continue;
      } else if (std::ispunct(c)) { // symbol(s)
        while (peek().has_value() && std::isgraph(c)) {
          b.push_back(absorb());
        }
        if (b == "(") {
          tokens.push_back( { .type = TokenType::parentheses_open } );
          b.clear();
        } else if (b == ")") {
          tokens.push_back( { .type = TokenType::parentheses_close } );
          b.clear();
        } else if (b == ",") {
          tokens.push_back( { .type = TokenType::comma } );
          b.clear();
        } else if (b == ":") {
          tokens.push_back( { .type = TokenType::colon } );
          b.clear();
        } else if (b == ";") {
          tokens.push_back( { .type = TokenType::semicolon } );
          b.clear();
        } else if (b == ".") {
          tokens.push_back( { .type = TokenType::period } );
          b.clear();
        } else if (b == "+") {
          tokens.push_back( { .type = TokenType::plus } );
          b.clear();
        } else if (b == "-") {
          tokens.push_back( { .type = TokenType::minus } );
          b.clear();
        // TODO: implement multiplication by juxtaposition of two expressions properly, for now its * since prototype implementation of it cause it to happen even upon variable declaration and it multiplied itself indefinitely
        } else if (b == "*") {
          tokens.push_back( { .type = TokenType::mult } );
          b.clear();
        } else if (b == "/") {
          tokens.push_back( { .type = TokenType::div } );
          b.clear();
        } else if (b == "^") {
          tokens.push_back( { .type = TokenType::power } );
          b.clear();
        } else if (b == "=") {
          tokens.push_back( { .type = TokenType::equals } );
          b.clear();
        } else if (b == "<") {
          tokens.push_back( { .type = TokenType::less } );
          b.clear();
        } else if (b == "<=") {
          tokens.push_back( { .type = TokenType::lessquals } );
          b.clear();
        } else if (b == ">") {
          tokens.push_back( { .type = TokenType::more } );
          b.clear();
        } else if (b == ">=") {
          tokens.push_back( { .type = TokenType::morequals } );
          b.clear();
        } else if (b == "!") {
          tokens.push_back( { .type = TokenType::excl } );
          b.clear();
        } else if (b == "&") {
          tokens.push_back( { .type = TokenType::et } );
          b.clear();
        } else if (b == "|") {
          tokens.push_back( { .type = TokenType::pipe } );
          b.clear();
        } else if (b == "$") {
          tokens.push_back( { .type = TokenType::dollar } );
          b.clear();
        } else if (b == "->") {
          tokens.push_back( { .type = TokenType::returns } );
          b.clear();
        } else if (b == "<-") {
          tokens.push_back( { .type = TokenType::gets } );
          b.clear();
        } else if (b == "[") {
          tokens.push_back( { .type = TokenType::box_open } );
          b.clear();
        } else if (b == "]") {
          tokens.push_back( { .type = TokenType::box_close } );
          b.clear();
        } else if (b == "#") {
          tokens.push_back( { .type = TokenType::octothorpe } );
          b.clear();
        } else {
          std::cerr << "Error: Unable to handle character(s) at " << m_index << std::endl;
          exit(1);
        }
      } else if (std::isdigit(c)) { // number
        b.push_back(absorb());
        while (peek().has_value() && std::isdigit(c)) {
          b.push_back(absorb());
        }
        tokens.push_back( { .type = TokenType::int_lit, .value = b } );
        b.clear();
      } else if (c == 0x5C) { // backslash, ansi escape character
        absorb();
        tokens.push_back( { .type = TokenType::escape } );
      } else if (c == 0x0A) { // end of line
        absorb();
        // tokens.push_back( { .type = TokenType::newline } );
      } else if (std::isspace(c)) { // space
        absorb();
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
