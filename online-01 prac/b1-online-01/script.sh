#!/bin/bash

IFS=$'\n'
tab_len=0

func() {
    cd "$1"
    for i in $(ls *.zip 2>/dev/null); do
        unzip $i >/dev/null
        rm $i
    done
    for i in $(ls); do
        for ((j = 0; j < tab_len; j++)); do
            echo -n "|   "
        done
        echo -n "|---"

        if [[ -d "$i" ]]; then
            # echo -n "|"

            echo "$i"
            tab_len=$((tab_len + 1))
            func "$i"
            tab_len=$((tab_len - 1))
        # elif [[ "$i" == *.zip ]]; then
        #     unzip "$i" >/dev/null
        #     rm "$i"
        #     # echo -n "|"

        #     echo ${i%.zip}
        #     tab_len=$((tab_len + 1))
        #     func "${i%.zip}"
        #     tab_len=$((tab_len - 1))
        else
            # echo -n "|"

            echo ${i}
        fi
    done
    cd ..

}

func "."
