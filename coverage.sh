#!/bin/bash

CFLAGS="--coverage -O0" LDFLAGS="-fprofile-arcs -ftest-coverage" make
mkdir cov
mv $(find . -name "*.gc*") coverage
cp $(find . -name "*.c") coverage
cd cov
lcov -d . -c -o cov_report
genhtml cov_report

