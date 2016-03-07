#!/usr/bin/env bash
make clean
make

make install
./external/run_main.sh -b
