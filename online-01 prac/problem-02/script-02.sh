#!/bin/bash

for filename in $(ls); do
    if [[ $filename != $0 ]]; then
        count=$(head -n $1 $filename | tail -1 | grep $2 | wc -l)
        if ((count > 0)); then
            echo "$filename is going to be deleted"
        fi
    fi
done
