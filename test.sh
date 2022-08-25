#!/bin/bash

# Trigger all your test cases with this script
./objdump_x2017 tests/sample_program.x2017  | diff - tests/sample_program.asm

./objdump_x2017 tests/sample1.x2017 | diff - tests/sample1.asm

./objdump_x2017 tests/sample2.x2017 | diff - tests/sample2.asm

./objdump_x2017 tests/sample3.x2017 | diff - tests/sample3.asm

./objdump_x2017 tests/sample4.x2017 | diff - tests/sample4.asm

./objdump_x2017 tests/sample5.x2017 | diff - tests/sample5.asm

./objdump_x2017 tests/sample6.x2017 | diff - tests/sample6.asm

./objdump_x2017 tests/sample7.x2017 | diff - tests/sample7.asm

./objdump_x2017 tests/sample8.x2017 | diff - tests/sample8.asm

./objdump_x2017 tests/sample9.x2017 | diff - tests/sample8.asm

./objdump_x2017 tests/sample10.x2017 | diff - tests/sample8.asm
