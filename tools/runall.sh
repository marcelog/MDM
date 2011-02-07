#!/bin/bash

rm /tmp/runall.tmp
touch /tmp/runall.tmp

let i=1
while [ "${i}" -le "${2}" ]; do
	echo Alcatel: $i
	time php example_Alcatel_73xx.php ${1} 34000 30000 300000 2>&1>>/tmp/runall.tmp&
	echo ZTE8426: $i
	time php example_ZTE_8426.php ${1} 34000 30000 300000 2>&1>>/tmp/runall.tmp&
	echo ZTE9xxx: $i
	time php example_ZTE_9xxx.php ${1} 34000 30000 300000 2>&1>>/tmp/runall.tmp&
	let i+=1
done
