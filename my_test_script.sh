#!/bin/sh

RED='\033[0;31m'
NC='\033[0m' # No Color

nruns=$1

declare -a runs=(
	'10 1 .1 .1 .1 12 0'
	'10 10 .01 .1 .1 12 0'
	'10 10 .1 .01 .01 12 0'
	'10 10 .05 .1 .1 12 0'
	'10 10 .02 .2 .01 12 0'
	'10 6 .02 .02 .02 12 0'
	'100 25 .02 .2 .01 12 0'
	'100 4 .01 .01 .01 12 0'
	);

	printf "\n${RED}Reoder Tests For Part 1, no output means it's working.${NC}\n"
	# for i in "${runs[@]}"
	# do
	# 	echo "\tTest: $i"
	# 	./elevator_part_1 $i | ./reorder | ./double-check
	# done

	printf "\n${RED}Reoder Tests For Part 2, no output means it's working.${NC}\n"
	for i in "${runs[@]}"
	do
		echo "\tTest: $i"
		./elevator_part_2 $i | ./reorder | ./double-check
	done

# printf "${RED}My elevator #1${NC}\n"
# for i in `seq 1 8`;
# do
# 	 ./run_and_time.sh ./elevator_part_1 $i $nruns 0
# done
#
# printf "\n${RED}His elevator #1${NC}\n"
# for i in `seq 1 8`;
# do
# 	 ./run_and_time.sh ./his_part_1 $i $nruns 0
# done

# printf "\n${RED}My elevator #2${NC}\n"
# for i in `seq 1 8`;
# do
# 	./run_and_time.sh ./elevator_part_2 $i $nruns 0
# done
#
# printf "\n${RED}His elevator #2${NC}\n"
# for i in `seq 1 8`;
# do
# 	./run_and_time.sh ./his_part_2 $i $nruns 0
# done

# printf "\n${RED}My elevator #3${NC}\n"
# for i in `seq 1 8`;
# do
# 	./run_and_time.sh ./elevator_part_2 $i $nruns 0
# done
