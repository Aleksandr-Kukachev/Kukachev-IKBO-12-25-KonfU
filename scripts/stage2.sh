#!/usr/bin/env sh
DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
EXE=${1:-"$DIR/../build/shell-emulator"}
"$EXE"
"$EXE" --vfs "$DIR/../data/minimal.csv"
"$EXE" --script "$DIR/stage2-start.txt"
"$EXE" --vfs "$DIR/../data/minimal.csv" --script "$DIR/stage2-start.txt"
"$EXE" --script
