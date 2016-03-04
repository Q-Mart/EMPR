#!/usr/bin/env bash
if [ $# -ne 1 ]; then
    echo "Invalid Number of Arguments"
    echo "Correct usage: ''./run_debug.sh records/RECORD_NAME''"
    exit
fi

cp $1 records/record

make clean
make debug

(./external/run_main.sh)&
pid=$!
sleep 1
./bin/main&
main_pid=$!
wait $pid
kill $main_pid
echo "Killed ./bin/main"
