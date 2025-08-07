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

echo "==== Running Standard_path_oram.sh ===="
bash shell/Standard_path_oram.sh > results/Standard_path_oram_result.txt

echo "==== Running forest_oram.sh ===="
bash shell/forest_oram.sh > results/forest_oram_result.txt

echo "==== Running opposite_path.sh ===="
bash shell/opposite_path.sh > results/opposite_path_result.txt

echo "==== Running opposite_path_forest.sh ===="
bash shell/opposite_path_forest.sh > results/opposite_path_forest_result.txt

echo "==== Running random_rr_forest.sh ===="
bash shell/random_rr_forest.sh > results/random_rr_forest_result.txt

echo "==== All scripts finished! ===="
