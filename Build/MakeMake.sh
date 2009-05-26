BUILD_DIR=$(dirname $0)
PROJECT_DIR=$(dirname ${BUILD_DIR})
DEFS=${BUILD_DIR}/Makefile.defs
RULES=${BUILD_DIR}/Makefile.rules

printf "## %s ##\n\n" "$(date -u)" > $DEFS
printf "## %s ##\n\n" "$(date -u)" > $RULES

for dir in ${PROJECT_DIR}/Source/main/*
do
	TGT=$(basename $dir | tr "a-z" "A-Z")
	
	echo "${TGT}_SRC := \\" >> ${DEFS}
	find $dir \( -name ".*" -prune \) -o \( -type "f" -name "*.cpp" -print \) | sed -e's/^/  /' -e's/$/ \\/'  >> ${DEFS}
	cat >> ${DEFS} << EOF
	
${TGT}_OBJ_ := \$(${TGT}_SRC:.cpp=.o)
${TGT}_OBJS := \$(foreach file, \$(${TGT}_OBJ_), \$(addprefix \$(OBJ_DIR)/, \$(notdir \$(file))))

${TGT}_DEP_ := \$(${TGT}_SRC:.cpp=_cpp.d)
${TGT}_DEPS := \$(foreach file, \$(${TGT}_DEP_), \$(addprefix \$(DEP_DIR)/, \$(notdir \$(file))))

DEPS      += \$(${TGT}_DEPS)
OBJS      += \$(${TGT}_OBJS)

#####################################

EOF

done

echo "PATHS += \\"                                                        >> ${DEFS}
find ${PROJECT_DIR}/Source/main/ -type d -name "*" -printf "\t%p \\\\\n"  >> ${DEFS}
echo ""                                                                   >> ${DEFS}