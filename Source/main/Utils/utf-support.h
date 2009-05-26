/*
	utf-support functions
	
	Copyright 1997, Hekkelman Programmatuur

*/

int mstrlen(const char *s);
void mstrcpy(char *dst, const char *src, int count);
int mcharlen(const char *s);
char *moffset(char *s, int count);
int municode(const char *s);
