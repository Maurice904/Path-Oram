#!/bin/bash

# $ chmod +x run_all.sh
# $ ./run_all.sh

store_file_1mil="testFiles/store_1000000"
store_file_200k="testFiles/store_200000"

echo "Creating necessary directories..."
mkdir -p plot/results
mkdir -p plot/csv
mkdir -p testFiles

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
chmod +x shell/*.sh

echo "==== Running Standard_path_oram.sh ===="
cd shell && bash Standard_path_oram.sh > ../plot/results/Standard_path_oram_result.txt && cd ..

echo "==== Running forest_oram.sh ===="
cd shell && bash forest_oram.sh > ../plot/results/forest_oram_result.txt && cd ..

echo "==== Running opposite_path.sh ===="
cd shell && bash opposite_path.sh > ../plot/results/opposite_path_result.txt && cd ..

echo "==== Running opposite_path_forest.sh ===="
cd shell && bash opposite_path_forest.sh > ../plot/results/opposite_path_forest_result.txt && cd ..

echo "==== Running random_rr_forest.sh ===="
cd shell && bash random_rr_forest.sh > ../plot/results/random_rr_forest_result.txt && cd ..

echo "==== All scripts finished! ===="