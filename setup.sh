#!/bin/bash

if [[ "$1" != "" ]]; then
    DIR="$1"
else
    echo "enter path"
fi


mkdir -p $DIR

ls -als $DIR
echo "" > "$DIR/log.txt"
chmod a+wrx "$DIR/log.txt"

ls -als $DIR
