#!/usr/bin/env bash
DIR=$(dirname "${BASH_SOURCE[0]}")
. $DIR/build.sh

. $DIR/venv/bin/activate
./$DIR/plot/main.py $1
