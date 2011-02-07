file=${1}
OLDIFS=$IFS
echo "------------------------"
echo -n "malloc: "
grep -ni allocated ${file} | wc -l
echo "------------------------"
echo -n "mfree:  "
grep -ni freeing ${file} | wc -l
echo "------------------------"
echo "Inconsistent alloced to free: "
tools/mm_trace.sh ${file} | grep \:1
echo "------------------------"
echo -n "Warns:        "
warns=`grep -ni "WARN - " ${file}`
let i=0
IFS=$'\n'
for j in ${warns}; do
	let i+=1
done
echo ${i}
if [ "${i}" -gt "0" ]; then
	for j in ${warns}; do
		echo ${j}
	done
fi
echo "------------------------"
echo -n "Nops:  "
grep -ni "Sent NOP" ${file} | wc -l
echo "------------------------"
echo -n "KeepAlives:  "
grep -ni "keepalive - Woke up" ${file} | wc -l
echo "------------------------"
echo -n "Execs:  "
grep -ni "mdm_device_exec" ${file} | grep -i start| wc -l
echo "------------------------"
echo -n "73xx Execs:  "
grep 73xx_exec ${file}  | grep -vi pre | grep -vi post | grep -i start  | grep -vi got | wc -l
echo "------------------------"
echo -n "8426 Execs:  "
grep 8426_exec ${file}  | grep -vi pre | grep -vi post | grep -i start  | grep -vi got | wc -l
echo "------------------------"
echo -n "9xxx Execs:  "
grep 9xxx_exec ${file}  | grep -vi pre | grep -vi post | grep -i start | grep -vi got | wc -l
echo "------------------------"
echo -n "Errors:       "
errors=`grep -n "ERROR - " ${file}`
let i=0
for j in ${errors}; do
	let i+=1
done
echo ${i}
if [ "${i}" -gt "0" ]; then
	for j in ${errors}; do
		echo ${j}
	done
fi
echo "------------------------"
IFS=$OLDIFS

