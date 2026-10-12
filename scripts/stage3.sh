#!/usr/bin/env sh
DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
EXE=${1:-"$DIR/../build/shell-emulator"}
"$EXE"
"$EXE" --vfs "$DIR/../data/minimal.csv" --script "$DIR/stage3-minimal.txt"
"$EXE" --vfs "$DIR/../data/files.csv" --script "$DIR/stage3-files.txt"
"$EXE" --vfs "$DIR/../data/deep.csv" --script "$DIR/stage3-start.txt"
"$EXE" --script "$DIR/stage3-exit.txt"
"$EXE" --vfs "$DIR/../data/missing.csv"
