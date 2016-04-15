#!/bin/sh

RED='\033[0;31m'
NC='\033[0m' # No Color

nruns=$1 

printf "${RED}My elevator #1${NC}\n"
for i in `seq 1 8`;
do
	echo #./run_and_time.sh ./elevator_part_1 $i $nruns 0
done     

printf "\n${RED}His elevator #1${NC}\n"
for i in `seq 1 8`;
do
	echo #./run_and_time.sh ./his_part_1 $i $nruns 0
done    

printf "\n${RED}My elevator #2${NC}\n"
for i in `seq 1 8`;
do
	./run_and_time.sh ./elevator_part_2 $i $nruns 0
done    

printf "\n${RED}His elevator #2${NC}\n"
for i in `seq 1 8`;
do
	./run_and_time.sh ./his_part_2 $i $nruns 0
done   

printf "\n${RED}My elevator #3${NC}\n"
for i in `seq 1 8`;
do
	#./run_and_time.sh ./elevator_part_2 $i $nruns 0
done    
