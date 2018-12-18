#!/bin/bash

CFLAGS="--coverage -O0" LDFLAGS="-fprofile-arcs -ftest-coverage" make
./bin/check_main
mkdir cov
mv $(find . -name "*.gc*") cov
cp $(find . -name "*.c") cov
cd cov
lcov -d . -c -o report
genhtml report

