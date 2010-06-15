#!/bin/sh

# A shell script to handle dependencies nicely

while [ $# -ge 1 ]
do
	case $1 in
	-dep)	DEPEND_FILE=$2
			shift
		;;
	-o) 	OUTPUT=$2
			shift
		;;
	*)  	FILES="${FILES} $1"
		;;
	esac
	shift
done

test $DEPEND_FILE || {
	echo "No dependency file given"
	exit 1
}
test $OUTPUT || {
	echo "No output file given"
	exit 1
}

printf "%s %s : %s \\\\\\n" "${DEPEND_FILE}" "${OUTPUT}" "${FILES}" > ${DEPEND_FILE}

for FILE in ${FILES}
do
	DIR=$(dirname $FILE)
	for INPUT in $(sed -e's#:.*##' ${FILE})
	do
		IN=${DIR}/${INPUT}
		[ -r ${IN} ] || IN=${INPUT}
		[ -r ${IN} ] || printf "%s: Warning: Cannot find file %s\\n" $0 $INPUT
		printf "\\t%s %c\\n" ${IN} "\\" >> ${DEPEND_FILE}
	done
done

printf "\\n" >> ${DEPEND_FILE}
