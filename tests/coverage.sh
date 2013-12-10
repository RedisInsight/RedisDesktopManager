#!/bin/sh
cp -Rf ./../redis-desktop-manager/include ./
cp -Rf ./../redis-desktop-manager/source ./

find . -name '*.cpp' -exec gcov -s ../redis-desktop-manager -o ./../bin/tests/obj {} \;

coveralls --exclude-pattern '.*\.h' --verbose --no-gcov