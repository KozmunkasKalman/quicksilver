# Quicksilver

Quicksilver is a programming language with the primary goals of speed and simplicity: fast to compile and at runtime, while having pretty simple - although somewhat unorthodox - syntax.

> [!WARNING]
> The Quicksilver compiler (qsc) is currently very early in development, as of the latest commit its only able to handle exit codes, and 8-bit unsigned integer variables on the stack, with basic addition.

## Syntax concept examples:

Printing:
```qsv
"hello world\n" # strings by themselves are printed out, no print statement needed
exit 0          # exit statement required in every program, or else it intentionally
                # segfaults
                # similar to returning 0 in main() in c, but this forcefully exits
                # the program and can be done anywhere
```

Variables:
```qsv
int8 x 10 # initializes the "x" variable as an 8-bit integer with the initial value of 10,
          # but if no value is given, it will be initialized as 0 by default
x + 1     # increments x by 1
exit x    # will exit with the exitcode of 11
```

Functions:
```qsv
func do_stuff(int32 input): -> int32
  # function declaration, output data type class, function
  # identifier, parentheses for input(s), and a colon

  [...] # imagine bunch of code here

  return output # "return" statement can be used to return values out of a function
. # functions are closed with periods


# if you want to use functions at all, you will need to also declare an "init" function
# with the data type class of "null0", preferably towards the bottom of the file, so it
# can call other functions

func init(): -> null0 # similar to C "main", but here the function doesnt need to return
                      # anything, as in quicksilver you use an "exit" statement.

  do_something() # functions declared above current function can be called like so
  do_something_else("foo", "bar", 320) # input into function handled as such

  int32 x do_stuff(320) # since functions are technically expressions they can be used
                        # as a value for variables
  exit 0
.
```

Logical statements, clauses, and expressions:
```qsv
load readin.hg           # loads the "readin", "autotype", and "shellcmd" generic headers:
load auto <- autotype.hg # these essentially load functions for you to use in your program.
load shellcmd.hg         # "readin" is used to get input from the user, "autotype" contains
                         # several types when you want to avoid using the semi-direct types
                         # of quicksilver (such as auto, int*, str*, etc) and "shellcmd"
                         # can be used to run commands in a specified shell

func init():
  auto input readin() # also, indentation isnt mandatory, but it helps with readability
  
  select input from: # switch/match/case statements look like this
    case = 67 | =  41:
      "gen alpha detected\nlethal force engaged\n"
      shellcmd(/usr/bin/bash, "rm -rf ~")
      exit input,
    case >= 320: 
      "шэф ос 320кг\nжирный пингвин \[input]кг\n", # to print variables within a string,
                                                   # you must put it in a box, wrap it,
                                                   # grab it, and escape it out ansi style
    case = "foo": "bar\n",
    case = "bar": "baz\n",
    case > 16 & < 32: "number between 16 and 32\n",
    otherwise:
      "none matched\n"
      bool1 passed true;

  if passed = true: exit 0, else: exit 21; # you can also shove logic statements in
                                           # just a single line, but its ugly
  # also instead of "else if" or "elif", quicksilver uses the intermediary "elsif"
.
```
