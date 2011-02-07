file=${1}
grep -i freeing ${file} | cut -d'x' -f3
