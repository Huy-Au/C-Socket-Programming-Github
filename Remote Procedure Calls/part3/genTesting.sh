#!/bin/bash

# Usage
# ./minTesting.sh [mindim] [maxdim] [increment factor] [weight]

count=$1
while [ $count -lt $2 ]
do
	echo "Length, width, height: "$count" with mass: "$4
	./box_2_client 137.154.149.28 $count $count $count $4
	count=`expr $count + $3`
	echo " "
done

