BSL=bsl
TMPFILE=/tmp/commands.$$

f_usage()
{
	printf "Usage: %s [-h] [-o <output>] <input>\\n" ;
}

while getopts "ho:" c
do
	case $c in
	o)	OUTFILE=$OPTARG
		;;
	h)	f_usage
		exit 0
		;;
	esac
done
shift $(expr $OPTIND - 1)

[ $OUTFILE -a -r $OUTFILE ] && rm $OUTFILE

for INPUT in $*
do
	if [ $OUTFILE ]
	then
		OUT=$OUTFILE
	else
		OUT=$(basename $INPUT ".bsl").rsrc
	fi
	
	[ -r ${INPUT} ] || {
		printf "Cannot read input file: %s\\n" ${INPUT}
		exit 1
	}

	DIR=$(dirname ${INPUT})
	
	echo "#!/bin/sh -x -e" > ${TMPFILE}

	## We can't pipe into Read (Grr BeOS...) so we hack
	sed -e"s#^\(.*\):\(.*\):\(.*\)#${BSL} -o ${OUT} -id \2 -name '\3' ${DIR}/\1#" -e'1s/-o/-nomerge -o/' ${INPUT} >> ${TMPFILE}
	chmod u+x ${TMPFILE}
	${TMPFILE} ; RC=$?
	rm ${TMPFILE}
done

exit $RC

# bsl -o Strings.rsrc -id 1 -name 'nans' nanstrs.txt
# bin/bsl -o $@ -id 2 -name "Format strings" Resources/fmtstrs.txt
