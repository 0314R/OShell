shell: scanner.l parser.y
	bison -d parser.y
	flex scanner.l
	gcc lex.yy.c parser.tab.c flexArray.c -o $@
