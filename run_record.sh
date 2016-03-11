#!/usr/bin/env bash
cp network_record.h include/network.h
make clean && make && make install
cp network_norecord.h include/network.h

. external/venv/bin/activate
cd external/plot
./main.py --record

