shell: scanner.l parser.y flexArray.c processManagement.c
	bison -d parser.y
	flex scanner.l
	gcc lex.yy.c parser.tab.c flexArray.c processManagement.c -o $@
