#!/bin/bash

change_ext() {
    cd $1
    for i in $(ls); do
        if [[ -d $i ]]; then
            change_ext "$i"
        elif [[ $i == *.cpp ]]; then
            echo "ji"
            mv $i ${i%.cpp}.c
        fi
    done
    cd ..
}

change_ext "."

for i in $(/usr/bin/tree); do
    echo $i
done
