# Quicksilver

Quicksilver is a programming language with the primary goals of speed and simplicity: fast to compile and at runtime, while having pretty simple - although somewhat unorthodox - syntax.

> [!WARNING]
> The Quicksilver compiler (qsc) is currently very early in development, as of the latest commit its only able to handle exit coes, and 1 byte unsigned integer variables on the stack.

## Syntax concept examples:

Printing:
```qsv
"hello world\n" #strings by themselves are printed out, no print statement needed
exit 0 #exit statement required in every program, or else it intentionally segfaults
```
printing inspired by HolyC
very similar to ``int main(){printf("hello world\n"); return 0;}`` in C, but the `exit` statement forcefully exits the program

Variables:
```qsv
int x 10 #initializes the "x" variable, with the initial value of 10
         #if no value given, will be 0 by default
x + 1 #increments x by 1
exit x #will exit with the exitcode of 11
```

If/elif/else statements:
```qsv
int x 67

if x = 67: #"=" instead of "==", since singular equals sign isnt used for setting variables
  "mango", #comma separator
if x < 67: #another "if" statement will basically act as an "else if" statement
  "mustard", #if not followed by another if statement, code after this comma will be treated as an "else" statement
  "shefos fat penguin 320kg"; #semicolon closes off if/while/switch statements
```

Functions:
```qsv
func int squareroot(int input): #function declaration, output data type class, function identifier,
                                #parentheses for input(s), colon
  flt half 0.5
  flt output input.flt()^half #floating point number variable "output" will be the input turned into a float, raised
                              #to the power of 0.5
  return output.int() #"return" statement can be used to return values out of a function
                      #in this case it turns the "output" float variable into an integer and returns that
. #functions are closed with periods


#if you want to use functions at all, you will need to also declare an "init" function with the data type
#class of "null", preferably towards the bottom of the file, so it can call other functions

func null init(): #similar to C "main", but here the function doesnt need to return anything, as in quicksilver
                  #you use an "exit" statement.

  myfirstfunction() #functions declared above current function can be called like so
  mysecondfunction(67, 41, "mango") #input into function handled as such

  int x square_root(9)
  exit x # should exit with 9
.```

Unsure how to handle variable value printing, currently the concept is similar to ansi escape codes (``\[variable name]``, ``"foo: \bar\n"``), but that makes specific variables (such as "n") unprintable, and the standard ``"${foo}\n"`` would be ugly, so I need to come up with a better solution for that.
