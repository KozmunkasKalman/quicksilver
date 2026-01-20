#!/bin/bash

set +e

g++ -std=c++23 -g -o bin/qsc-debug src/main.cpp
gdb --args bin/qsc-debug test.qsv
