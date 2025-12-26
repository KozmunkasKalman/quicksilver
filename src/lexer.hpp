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
  load, // <input_file> - loads a files contents, usually .hg quicksilver generic headers,
        //                can also be used to load files contents as an expression
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
      // first check if its a whitespace, if so then ignore it
      if (std::isspace(peek().value())) { // space
        absorb();
      } else if (peek().value() == 0x0A) { // end of line
        absorb();
      // then check for keywords
      } else if (std::isalpha(peek().value())) {
        b.push_back(absorb());
        while (peek().has_value() && std::isalnum(peek().value())) {
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
      // then check for character strings
      } else if (peek().value() == '"') { // string
        absorb();
        while (peek().has_value() && peek().value() != '"') {
          if (peek().value() != EOF) {
            b.push_back(absorb());
          } else {
            std::cerr << "Error: String was never closed off" << std::endl;
            exit(1);
          }
          tokens.push_back( { .type = TokenType::str_lit, .value = b } );
          b.clear();
        }
        absorb();
        tokens.push_back( { .type = TokenType::str_lit, .value = b } );
        b.clear();
        continue;
      // then check for numbers
      } else if (std::isdigit(peek().value())) { // number
        b.push_back(absorb());
        while (peek().has_value() && std::isdigit(peek().value())) {
          b.push_back(absorb());
        }
        tokens.push_back( { .type = TokenType::int_lit, .value = b } );
        b.clear();
      // then check for single character symbols
      } else if (peek().value() == '(') {
        absorb();
        tokens.push_back( { .type = TokenType::parentheses_open } );
      } else if (peek().value() == ')') {
        absorb();
        tokens.push_back( { .type = TokenType::parentheses_close } );
      } else if (peek().value() == ',') {
        absorb();
        tokens.push_back( { .type = TokenType::comma } );
      } else if (peek().value() == ':') {
        absorb();
        tokens.push_back( { .type = TokenType::colon } );
      } else if (peek().value() == ';') {
        absorb();
        tokens.push_back( { .type = TokenType::semicolon } );
      } else if (peek().value() == '.') {
        absorb();
        tokens.push_back( { .type = TokenType::period } );
      } else if (peek().value() == '+') {
        absorb();
        tokens.push_back( { .type = TokenType::plus } );
      } else if (peek().value() == '-') {
        absorb();
        tokens.push_back( { .type = TokenType::minus } );
      // TODO: imclement multiplication by juxtaposition of two expressions properly, for now its * since prototype implementation of it cause it to happen even upon variable declaration and it multiplied itself indefinitely
      } else if (peek().value() == '*') {
        absorb();
        tokens.push_back( { .type = TokenType::mult } );
      } else if (peek().value() == '/') {
        absorb();
        tokens.push_back( { .type = TokenType::div } );
      } else if (peek().value() == '=') {
        absorb();
        tokens.push_back( { .type = TokenType::power } );
      } else if (peek().value() == '=') {
        absorb();
        tokens.push_back( { .type = TokenType::equals } );
      } else if (peek().value() == '<') {
        absorb();
        tokens.push_back( { .type = TokenType::less } );
      } else if (peek().value() == '>') {
        absorb();
        tokens.push_back( { .type = TokenType::more } );
      } else if (peek().value() == '!') {
        absorb();
        tokens.push_back( { .type = TokenType::excl } );
      } else if (peek().value() == '&') {
        absorb();
        tokens.push_back( { .type = TokenType::et } );
      } else if (peek().value() == '|') {
        absorb();
        tokens.push_back( { .type = TokenType::pipe } );
      } else if (peek().value() == '$') {
        absorb();
        tokens.push_back( { .type = TokenType::dollar } );
      } else if (peek().value() == '[') {
        absorb();
        tokens.push_back( { .type = TokenType::box_open } );
      } else if (peek().value() == ']') {
        absorb();
        tokens.push_back( { .type = TokenType::box_close } );
      } else if (peek().value() == '#') {
        absorb();
        tokens.push_back( { .type = TokenType::octothorpe } );
      } else if (peek().value() == 0x5C) { // '\'
        absorb();
        tokens.push_back( { .type = TokenType::escape } );
      // then check for multi character symbols
      } else if (std::ispunct(peek().value())) { // symbol(s)
        while (peek().has_value() && std::isgraph(peek().value())) {
          b.push_back(absorb());
        }
        if (b == "<=") {
          tokens.push_back( { .type = TokenType::lessquals } );
          b.clear();
        } else if (b == ">=") {
          tokens.push_back( { .type = TokenType::morequals } );
          b.clear();
        } else if (b == "->") {
          tokens.push_back( { .type = TokenType::returns } );
          b.clear();
        } else if (b == "<-") {
          tokens.push_back( { .type = TokenType::gets } );
          b.clear();
        }
      // if neither matched throw an error
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
