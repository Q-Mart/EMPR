#!/usr/bin/env bash
DIR=$(dirname "${BASH_SOURCE[0]}")

# make virtualenv only if one doesn't already exist
if [ ! -d $DIR/venv/ ]; then
  virtualenv $DIR/venv/
fi

source $DIR/venv/bin/activate
pip install -r $DIR/requirements.txt
