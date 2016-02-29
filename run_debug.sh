#!/usr/bin/env bash
make clean
make debug

./external/build.sh
(./external/plot/main.py --debug)&
pid=$!
sleep 1
./bin/main&
main_pid=$!
wait $pid
kill $main_pid
echo "Killed ./bin/main"
