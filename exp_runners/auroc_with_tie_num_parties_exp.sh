#!/usr/bin/env bash
rm results/auroc_with_tie/auroc_with_tie_num_parties_*
nparties=(2 4 8)
nrepetition=1
for (( iter=0; iter<$nrepetition; iter++))
do
	for i in ${nparties[@]};
	do
		s="$i"
		for (( c=0; c<$i; c++ ))  
		do
			s="${s},1000"
		done
		echo " =========== Iteration: $iter - Num of Parties: $i =========== "
		./../cmake-build-debug/helper "127.0.0.1" 7777 >> results/auroc_with_tie/auroc_with_tie_num_parties_helper &
		sleep 1
		./../cmake-build-debug/proxy_auroc_tie 0 9999 "127.0.0.1" 7777 "127.0.0.1" 1 $s >> results/auroc_with_tie/auroc_with_tie_num_parties_p1 &
		sleep 1
		./../cmake-build-debug/proxy_auroc_tie 1 9999 "127.0.0.1" 7777 "127.0.0.1" 1 $s >> results/auroc_with_tie/auroc_with_tie_num_parties_p2 &
		wait 
	done
done
