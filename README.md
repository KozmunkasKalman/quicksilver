# Quicksilver

Quicksilver is a programming language with the primary goals of speed and simplicity: fast to compile and at runtime, while having pretty simple - although somewhat unorthodox - syntax.

> [!WARNING]
> The Quicksilver compiler (qsc) is currently very early in development, as of the latest commit its only able to handle exit coes, and 1 byte unsigned integer variables on the stack.

## Syntax concept examples:

Returning an exitcode:
``func init(): 
  exit 0
.``
very similar to "int main(){return 0;}" in C, but it forcefully exits the program

Variables:
``func init():
  var x 10 #initializes the "x" variable, with the initial value of 10
           #if no value given, will be 0 by default
  x + 1 #increments x by 1
  exit x #will exit with the exitcode of 11
.``

Printing:
``func init():
  "hello world\n"
  exit 0
.``
inspired by HolyC

If/elif/else statements:
``func init():
  var x 67
  
  if x = 67: #"=" instead of "==", since singular equals sign isnt used for setting variables
    "mango"
  , if x < 67: #basically an "else if" statement
    "mustard"
  , #basically the "else" statement
    "shefos fat penguin 320kg"
  ;

  var y = 41

  #can also be reformatted like the following:
  if x = 41: "mango", #"if"
  if x > 41: "mustard", #"else if"
  "sheofs fat penguin 320kg"; #"else"
.``
commas are used as "separators", and semicolons as closure to statements

Kind of unsure if I should keep `var` for variables, or use separate data type declarations, such as `int` (signed integer), `pint` (unsigned [positive] integer), ``flt`` (floating point numbers), ``str`` (character string), ``null`` (basically HolyC U0, or Rust !), etc.

Also unsure of how to handle variable value printing, currently the concept is similar to ansi escape codes (\[variable name], "foo: \bar\n"), but that makes specific variables (such as "n") unprintable, and the standard "${foo}\n" would be ugly, so I need to come up with a better solution for that.
