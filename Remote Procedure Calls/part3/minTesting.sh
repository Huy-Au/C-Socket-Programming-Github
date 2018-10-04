#!/bin/bash

# Usage
# ./minTesting.sh [dim] [minweight] [maxweight] [increment factor]

count=$2
while [ $count -lt $3 ]
do
	echo "Length, width, height: 1 with mass: "$count
	./box_2_client 137.154.149.28 $1 $1 $1 $count
	count=`expr $count + $4`
	echo " "
done
