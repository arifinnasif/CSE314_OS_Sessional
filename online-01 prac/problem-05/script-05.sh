#!/bin/bash

test_num=0
while true; do
    if ! bash rand.sh >out.txt 2>err.txt; then
        echo $test_num
        cat out.txt
        cat err.txt
        break
    fi
    test_num=$((test_num + 1))
done
