sum-it.rsrc: sum-it.r sum-it.menus
	../bin/mb -o sum-it.rsrc sum-it.menus
	../bin/bsl -o sum-it.rsrc -id 1 -header ../Sources/nanstrs.h \
		-name "nans" nanstrs.txt
	../bin/bsl -o sum-it.rsrc -id 2 -header ../Sources/fmtstrs.h \
		-name "Format strings" fmtstrs.txt
	../bin/bsl -o sum-it.rsrc -id 3 -header ../Sources/undostrs.h \
		-name "Undo/Redo strings" undostrs.txt
#	../bin/bsl -o sum-it.rsrc -id 4 -header ../Sources/errmsg.h \
#		-name "Error Messages" errmsg.txt
	../bin/bsl -o sum-it.rsrc -id 5 -header ../Sources/res5.h \
		-name "res5" res5.txt
	../bin/bsl -o sum-it.rsrc -id 6 -header ../Sources/errmsg.h \
		-name "Error Messages" errmsg.txt
	../bin/bsl -o sum-it.rsrc -id 7 -name "FunctionNames" FuncNames.txt
	../bin/bsl -o sum-it.rsrc -id 8 -name "FuncDescs" FuncDescs.txt
	../bin/bsl -o sum-it.rsrc -id 6 -header ../Sources/usrmsg.h \
		-name "User Messages" usrmsg.txt
	../bin/bsl -o sum-it.rsrc -id 6 -header ../Sources/errmsg.h \
		-name "Error Messages" errmsg.txt

About.rsrc:
	mwbres -o About.rsrc -nomerge About.r

icons.rsrc: icons/cancel.icon.raw \
	icons/cancel_p.icon.raw \
	icons/ok.icon.raw \
	icons/ok_p.icon.raw \
	icons/sum.icon.raw \
	icons/sum_p.icon.raw 
	mwbres -nomerge -o icons.rsrc icons.r

icons/cancel.icon.raw: icons/cancel.icon
	dumpicon icons/cancel.icon

icons/cancel_p.icon.raw: icons/cancel_p.icon
	dumpicon icons/cancel_p.icon

icons/ok.icon.raw: icons/ok.icon
	dumpicon icons/ok.icon

icons/ok_p.icon.raw: icons/ok_p.icon
	dumpicon icons/ok_p.icon

icons/sum.icon.raw: icons/sum.icon
	dumpicon icons/sum.icon

icons/sum_p.icon.raw: icons/sum_p.icon
	dumpicon icons/sum_p.icon
