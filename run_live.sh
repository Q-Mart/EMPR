#!/usr/bin/env bash
make clean
make

make install
./external/build.sh
./external/plot/main.py
