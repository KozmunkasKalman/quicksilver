#!/bin/bash

set +e

g++ -std=c++23 -g -o build/qsc-debug src/main.cpp
gdb --args build/qsc-debug test.qsv
