#!/bin/bash
# 3. random read ratio + forest : 1 + 2
# store <testFiles> -s --r 0.5
# operate <testFiles> -s --r 0.5


# $ chmod +x random_rr_forest.sh
# $ ./random_rr_forest.sh > random_rr_forest_result.txt
csv_file="result_random_rr_forest.csv"

echo "operate_size,tree_size,avg_stash,min_stash,max_stash,avg_time,min_time,max_time" > "$csv_file"

operate_sizes=(100000 200000 500000 700000 1000000)
tree_sizes=(200000 1000000)

mkdir -p testFiles


store_file_1mil="testFiles/store_1000000"
store_file_200k="testFiles/store_200000"

for operate_size in "${operate_sizes[@]}"; do
    operate_file="testFiles/operate_temp"
    echo "Generating operation file..."
    python3 scripts/fileGen.py operation $operate_size --output "$operate_file"
    for tree_size in "${tree_sizes[@]}"; do
        store_file="testFiles/store_${tree_size}"   
        echo "=== Testing Operation Size $operate_size and Data Size: $tree_size ==="
        time_arr=()
        total_time=0
        stash_sizes=()

        declare -a unique_prints=()
        unique_count=0
        max_size=$((tree_size + 1))

        for i in {1..10}; do
            echo "  [Run $i]"
            start_time=$(date +%s%N)

            stash_output=$(./path_oram <<EOF
store $store_file -s --r 0.5
operate $operate_file -s --r 0.5
print sizes
exit
EOF
            )
            end_time=$(date +%s%N)
            elapsed_ns=$((end_time - start_time))
            elapsed_ms=$((elapsed_ns / 1000000))
            total_time=$((total_time + elapsed_ms))
            time_arr+=($elapsed_ms)



            current_print_sizes=$(echo "$stash_output" | awk '/Forest has/{flag=1} flag')

            is_new=1
            for prev in "${unique_prints[@]}"; do
                if [[ "$current_print_sizes" == "$prev" ]]; then
                    is_new=0
                    break
                fi
            done

            if (( is_new )); then
                unique_prints+=("$current_print_sizes")
                unique_count=$((unique_count+1))
            fi


            stash_line=$(echo "$stash_output" | grep "Tree\[0\]")
            stash_size=$(echo "$stash_line" | awk '{print $NF}')
            stash_sizes+=($stash_size)


            min_time=${time_arr[0]}
            max_time=${time_arr[0]}
            sum_time=0

            for t in "${time_arr[@]}"; do
                if (( t < min_time )); then min_time=$t; fi
                if (( t > max_time )); then max_time=$t; fi
                ((sum_time += t))
            done
            avg_time=$((sum_time / 10))

            echo "    Time: ${elapsed_ms} ms | Stash Size: $stash_size"
        done

        if (( unique_count == 1 )); then
            echo "[print sizes] (repeated 10 times, only shown once):"
            echo "${unique_prints[0]}"
        else
            echo "[print sizes] (There are ${unique_count} results: ):"
            for ((k=0; k<unique_count; k++)); do
                echo
                echo "[unique #$((k+1))]:"
                echo "${unique_prints[k]}"
            done
        fi
        
        min_stash=${stash_sizes[0]}
        max_stash=${stash_sizes[0]}
        sum_stash=0

        for s in "${stash_sizes[@]}"; do
            if (( s < min_stash )); then
                min_stash=$s
            fi
            if (( s > max_stash )); then
                max_stash=$s
            fi
            ((sum_stash += s))
        done

        avg_stash=$((sum_stash / 10))
        avg_time=$((total_time / 10))

        echo "===> [Summary for size : $tree_size and operation size : $operate_size]:"
        echo "     Avg Time   : $avg_time ms"
        echo "     Stash Size : avg=$avg_stash, min=$min_stash, max=$max_stash"
        echo

        echo "$operate_size,$tree_size,$avg_stash,$min_stash,$max_stash,$avg_time,$min_time,$max_time" >> "$csv_file"
    done
done