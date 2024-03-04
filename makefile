CC = gcc
CFLAGS = -I.

all: driver.c parser.c parseTreeStack.h parserDef.h lexer.h lexerDef.h lexer.c stack.h stack.c hashtable.h hashtable.c
	$(CC) driver.c parser.c parseTreeStack.h parserDef.h lexer.h lexerDef.h lexer.c stack.h stack.c hashtable.h hashtable.c