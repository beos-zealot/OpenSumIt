#!/bin/sh

# A shell script to handle dependencies nicely

while [ $# -ge 1 ]
do
	case $1 in
	-odir)	OBJ_DIR=$2
			shift
		;;
	-dep)	DEPEND_FILE=$2
			shift
		;;
	-pwd)	WORK_DIR=$2"/"
			shift
		;;
	*)	GCC_ARGS="$GCC_ARGS $1"
		;;
	esac
	shift
done

test $DEPEND_FILE || {
	echo "No dependency file given"
	exit 1
}

test $OBJ_DIR || {
	echo "No object directory given"
	exit 1
}

# Remove all includes beginning with a '/'
# Replace the '.o' with the dependency file AND the .o in the correct folder
# Turn all relative directories into absolutes
gcc -MM -MG $GCC_ARGS | \
    sed -e 's@ /[^ ]*@@g' \
        -e "s@^\(.*\)\.o *: *@${OBJ_DIR}/\1.o ${DEPEND_FILE} : @" | \
              grep -v "^ \\\\$" | sed -e"s@ \([^/]\)@ ${WORK_DIR}\1@g" > $DEPEND_FILE
