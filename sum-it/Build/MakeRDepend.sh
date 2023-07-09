#!/bin/sh

# A shell script to handle dependencies nicely

while [ $# -ge 1 ]
do
	case $1 in
	-dep)	DEPEND_FILE=$2
			shift
		;;
	*)	REZ_ARGS="$REZ_ARGS $1"
		;;
	esac
	shift
done

test $DEPEND_FILE || {
	echo "No dependency file given"
	exit 1
}

printf "%s " "${DEPEND_FILE}:" > ${DEPEND_FILE}

# Remove all includes beginning with a '/'
# Replace the '.o' with the dependency file AND the .o in the correct folder
# Turn all relative directories into absolutes
# printf "%s " "${DEPEND_FILE}" > ${DEPEND_FILE}
#rez -m -p ${REZ_ARGS} >> ${DEPEND_FILE} && exit 0

#rm ${DEPEND_FILE}
#exit 1
