#!/usr/bin/env sh
DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
EXE=${1:-"$DIR/../build/shell-emulator"}
for script in "$DIR"/errors/*.txt; do
    "$EXE" --vfs "$DIR/../data/deep.csv" --script "$script"
done
