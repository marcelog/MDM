#!/bin/bash
file=${1}
tools/mm_debug_get_freed.sh ${file} > /tmp/freed
tools/mm_debug_get_allocated.sh ${file} > /tmp/allocated
for i in `cat /tmp/allocated | sort -u`; do
	echo -n "A:${i}:"
	total=`grep ${i} ${file} | wc -l | tr -d "\t"`
	echo -n "$total:"
	mod=$(($total % 2))
	if [ ! "${mod}" -eq "0" ]; then
		echo "1"
	else
		echo "0"
	fi
done
for i in `cat /tmp/freed | sort -u`; do
	echo -n "F:${i}:"
	total=`grep ${i} ${file} | wc -l | tr -d "\t"`
	echo -n "$total:"
	mod=$(($total % 2))
	if [ ! "${mod}" -eq "0" ]; then
		echo "1"
	else
		echo "0"
	fi
done
