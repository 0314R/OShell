shell: scanner.l parser.y flexArray.c
	bison -d parser.y
	flex scanner.l
	gcc lex.yy.c parser.tab.c flexArray.c -o $@
