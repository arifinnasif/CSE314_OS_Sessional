#!/bin/bash

total_marks=100
stud_count=5
[[ ! -f "AcceptedOutput.txt" ]] && exit 1

declare -A marks

if [[ $# -ge 1 && $# -le 2 ]]; then
    total_marks=$1
    if [[ $# -eq 2 ]]; then
        stud_count=$2
    fi
fi

# echo $total_marks
# echo $stud_count

if cd Submissions/; then
    for ((i = 0; i < stud_count; i++)); do
        stud_id=180512$((i + 1))
        if [[ -d $stud_id ]]; then
            cd $stud_id
            # pwd
            if [[ -f "$stud_id.sh" ]]; then
                echo "testing $stud_id.sh"
                bash "$stud_id.sh" >"../out.txt"
                unmatched_line_count=$(diff --ignore-all-space ../out.txt ../../AcceptedOutput.txt | grep --extended-regexp "(<|>)" | wc -l)
                marks["$stud_id"]=$((total_marks - unmatched_line_count * 5))

                if ((marks["$stud_id"] < 0)); then
                    marks["$stud_id"]=0
                fi

                [[ -e "../out.txt" ]] && rm "../out.txt"
                matched_with_output+=("$stud_id")
            else
                marks["$stud_id"]=0
            fi

            cd ..

        else
            marks["$stud_id"]=0
        fi
    done

    # for (( i=0; i<stud_count; i++ )); do
    #     temp=180512$(( i+1 ))
    #     echo "$temp,${marks[$temp]}"
    # done

    arr_len=${#matched_with_output[@]}
    for ((i = 0; i < arr_len; i++)); do
        for ((j = i + 1; j < arr_len; j++)); do
            echo "checking ${matched_with_output[$i]} ${matched_with_output[$j]}"
            candidate1=${matched_with_output[$i]}
            candidate2=${matched_with_output[$j]}

            cc_unmatched_line_count=$(diff --ignore-trailing-space --ignore-blank-lines $candidate1/$candidate1.sh $candidate2/$candidate2.sh | wc -l)
            if ((cc_unmatched_line_count == 0)); then
                echo "copy found"
                marks[$candidate1]=$((-marks[$candidate1]))
                marks[$candidate2]=$((-marks[$candidate2]))
            fi
        done
    done

    # for ((i = 0; i < stud_count; i++)); do
    #     temp=180512$((i + 1))
    #     echo "$temp,${marks[$temp]}"
    # done

    cd ..
fi

echo "student_id,score" >output.csv
for ((i = 0; i < stud_count; i++)); do
    temp=180512$((i + 1))
    echo "$temp,${marks[$temp]}" >>output.csv
done
