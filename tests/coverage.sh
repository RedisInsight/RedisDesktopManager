#!/bin/sh
cp -Rf ./../src/app ./
cp -Rf ./../src/modules ./

find . -name '*.cpp' -exec gcov -s ../src -o ./../bin/tests/obj {} \;
