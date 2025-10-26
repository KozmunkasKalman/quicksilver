#!/bin/bash

set +e

g++ -g -o build/qsc-debug src/main.cpp
gdb --args build/qsc-debug test.qsv
