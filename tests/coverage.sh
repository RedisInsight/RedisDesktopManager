#!/bin/sh
cd ./../src

find . -name '*.cpp' -exec gcov -s `pwd` -o ./../bin/tests/obj {} \;
