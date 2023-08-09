#!/bin/bash

file_des_arr=()

get_file_des() {
    len=${#file_des_arr[@]}
    for ((i = 0; i < len; i++)); do
        # echo ${file_des_arr[$i]}
        if [[ ${file_des_arr[$i]} == $1 ]]; then
            echo $i
            return
        fi
    done

    echo "-1"
    return
}

get_min_fd() {
    len=${#file_des_arr[@]}
    for ((i = 0; i < len; i++)); do
        # echo ${file_des_arr[$i]}
        if [[ ${file_des_arr[$i]} == "" ]]; then
            echo $i
            return
        fi
    done

    echo $len
    return
}

# echo $(get_file_des "foo")
# get_file_des "bar"

read Q

for ((i = 0; i < $Q; i++)); do
    read line
    cmd=($line)
    if [[ ${cmd[0]} == "open" ]]; then
        fd=$(get_file_des "${cmd[1]}")
        if ((fd == -1)); then
            temp=$(get_min_fd)
            file_des_arr[$temp]="${cmd[1]}"
            if [[ ! -f "${cmd[1]}" ]]; then
                touch "${cmd[1]}"
            fi
            # echo ${file_des_arr[@]}
        fi
    elif [[ ${cmd[0]} == "close" ]]; then
        fd=$(get_file_des "${cmd[1]}")
        if ((fd != -1)); then
            file_des_arr[$fd]=""

        fi
    elif [[ ${cmd[0]} == "append" ]]; then
        cmd_fd=${cmd[1]}
        echo "${cmd[2]}" >>${file_des_arr[$cmd_fd]}
    fi

    echo ${!file_des_arr[@]}
done
