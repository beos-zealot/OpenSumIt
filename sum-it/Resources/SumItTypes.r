Type 'DLOG' {
	longint nonmodal = 'dlog', modal = 'mdlg';
	rect;														// rectangle
	cstring;													// title
	array {
		switch {
			case Button:
				key longint = 'btn ';
				rect;
				cstring;
				cstring;
				longint;
			case Tabbook:
				key longint = 'tabs';
				rect;
				cstring;
			case Tabsheet:
				key longint = 'shyt';
				rect;
				cstring;
			case Box:
				key longint = 'box ';
				rect;
				cstring;
			case Popup:
				key longint = 'ppup';
				rect;
				cstring;
				cstring;
				longint;
			case TextEdit:
				key longint = 'txte';
				rect;
				cstring;
				cstring;
				cstring;
				cstring;
				longint;
			case Checkbox:
				key longint = 'chkb';
				rect;
				cstring;
				cstring;
				longint;
			case Radiobutton:
				key longint = 'rdbt';
				rect;
				cstring;
				cstring;
				longint;
			case View:
				key longint = 'view';
				rect;
				cstring;
			case List:
				key longint = 'list';
				rect;
				cstring;
			case Caption:
				key longint = 'sttx';
				rect;
				cstring;
				cstring;
			case TextView:
				key longint = 'txtv';
				rect;
				cstring;
			case User:
				key longint = 'user';
				rect;
				cstring;
				longint;
			case End:
				key longint = 'end ';
		};
	};
	longint = 0;
};
