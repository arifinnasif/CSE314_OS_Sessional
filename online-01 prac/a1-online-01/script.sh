#!/bin/bash

IFS=$'\n'

for i in $(ls); do
    if [[ ! -f "$i" ]]; then
        continue
    fi
    if [[ "$i" == $0 ]]; then
        continue
    fi
    director_name=$(cat "$i" | tail -n2 | head -n1)
    mkdir -p "$director_name"
    mv "$i" "$director_name/$i"

done
