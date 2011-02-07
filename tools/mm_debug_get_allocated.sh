file=${1}
grep -i allocated ${file} | cut -d'x' -f3
