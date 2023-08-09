#!/bin/bash

DIR=

traverse_dir() {
    cd $1
    echo "HELLO"
    IFS=$'\n'
    for i in $(ls); do
        if [[ -d "$i" ]]; then
            traverse_dir "$i"
        fi
        if [[ "$DIR" == "$(pwd)" && "$i" == "$0" ]]; then
            continue
        fi
        player=$(head -n1 "$i")
        country=$(head -n2 "$i" | tail -n1)
        role=$(head -n4 "$i" | tail -n1)

        mkdir -p "$DIR/$country/$role"
        mv "$i" "$DIR/$country/$role/$player.txt"

        echo $player
    done

    cd ..
    filecount=$(ls "$1" | wc -l)
    if [[ $filecount -eq 0 ]]; then
        rmdir "$i"
    fi
}

DIR="$(pwd)"

traverse_dir "."
