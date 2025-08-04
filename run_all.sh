#!/bin/bash

# $ chmod +x run_all.sh
# $ ./run_all.sh

chmod +x *.sh

echo "==== Running no_opt.sh ===="
bash no_opt.sh > no_opt_result.txt

echo "==== Running forest_opt.sh ===="
bash forest_opt.sh > forest_opt_result.txt

echo "==== Running ring_oram.sh ===="
bash ring_oram.sh > ring_oram_result.txt

echo "==== Running ring_oram_forest.sh ===="
bash ring_oram_forest.sh > ring_oram_forest_result.txt

echo "==== Running random_rr_forest.sh ===="
bash random_rr_forest.sh > random_rr_forest_result.txt

echo "==== All scripts finished! ===="
