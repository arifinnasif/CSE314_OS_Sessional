#!/bin/bash

DIR=

traverse_dir() {
    cd $1

    IFS=$'\n'
    for i in $(ls); do
        if [[ -d "$i" ]]; then
            traverse_dir "$i"
            continue
        fi
        if [[ "$DIR" == "$(pwd)" && "$i" == "$0" ]]; then
            continue
        fi
        pwd
        player=$(head -n1 "$i")
        country=$(head -n2 "$i" | tail -n1)
        role=$(head -n4 "$i" | tail -n1)

        mkdir -p "$DIR/$country/$role"
        mv "$i" "$DIR/$country/$role/$player.txt"

    done

    cd ..
    filecount=$(ls "$1" | wc -l)
    if [[ $filecount -eq 0 ]]; then
        rmdir "$1"
    fi
}

DIR="$(pwd)"

traverse_dir "."
