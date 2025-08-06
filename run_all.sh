#!/bin/bash

# $ chmod +x run_all.sh
# $ ./run_all.sh

store_file_1mil="testFiles/store_1000000"
store_file_200k="testFiles/store_200000"

echo "Generating storage file..."   
python3 scripts/fileGen.py storage 1000000 --output "$store_file_1mil"
python3 scripts/fileGen.py storage 200000  --output "$store_file_200k"

echo "Generating operation file..."   
python3 scripts/fileGen.py operation 100000 --output "testFiles/operate_100000"
python3 scripts/fileGen.py operation 200000 --output "testFiles/operate_200000"
python3 scripts/fileGen.py operation 500000 --output "testFiles/operate_500000"
python3 scripts/fileGen.py operation 700000 --output "testFiles/operate_700000"
python3 scripts/fileGen.py operation 1000000 --output "testFiles/operate_1000000"

chmod +x *.sh

echo "==== Running no_opt.sh ===="
bash no_opt.sh > results/no_opt_result.txt

echo "==== Running forest_opt.sh ===="
bash forest_opt.sh > results/forest_opt_result.txt

echo "==== Running ring_oram.sh ===="
bash ring_oram.sh > results/ring_oram_result.txt

echo "==== Running ring_oram_forest.sh ===="
bash ring_oram_forest.sh > results/ring_oram_forest_result.txt

echo "==== Running random_rr_forest.sh ===="
bash random_rr_forest.sh > results/random_rr_forest_result.txt

echo "==== All scripts finished! ===="
