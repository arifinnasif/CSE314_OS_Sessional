#!/bin/bash

for filename in $(ls); do
    if [[ ! -d $filename && $filename != $0 && $filename =~ [0-9] ]]; then
        echo "deleting $filename"
    fi
done
