#!/usr/bin/env bash
rm results/auroc_with_tie/auroc_with_tie_unbalanced_*
s="8,12,18,32,58,107,258,507,1008"
nrepetition=1
for (( iter=0; iter<$nrepetition; iter++))
do
	echo " ======== Iteration $iter ========== "
	./../cmake-build-debug/helper "127.0.0.1" 7777 >> results/auroc_with_tie/auroc_with_tie_unbalanced_helper &
	sleep 1
	./../cmake-build-debug/proxy_auroc_tie 0 9999 "127.0.0.1" 7777 "127.0.0.1" 1 $s >> results/auroc_with_tie/auroc_with_tie_unbalanced_p1 &
	sleep 1
	./../cmake-build-debug/proxy_auroc_tie 1 9999 "127.0.0.1" 7777 "127.0.0.1" 1 $s >> results/auroc_with_tie/auroc_with_tie_unbalanced_p2 &
	wait 
done
