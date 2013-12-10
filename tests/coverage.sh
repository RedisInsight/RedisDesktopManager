#!/bin/sh
cp -Rf ./../redis-desktop-manager/include ./
cp -Rf ./../redis-desktop-manager/source ./

find . -name '*.cpp' -exec gcov -o ./../bin/tests/obj {} \;

coveralls --verbose --no-gcov -t mW1rgPRX8DekvC1GxbUrENH7We8G09BgH