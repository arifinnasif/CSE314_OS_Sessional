#!/bin/bash

filecount=$#

for i in $@; do
    if [[ ! -e $i ]]; then
        echo "$i does not exist"
    elif [[ ! -f $i ]]; then
        echo "$i not a regular file"
    elif [[ $i == $0 ]]; then
        echo "$i is being executed. cannot remove exec permission"
    elif [[ ! -x $i ]]; then
        echo "$i is already not executable"
    else
        echo "$i's exec permission is being removed"
        chmod -x $i
    fi
done
