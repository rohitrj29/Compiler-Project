gcc-11 -c lexer.c -o lexer.o
gcc-11 -c stack.c -o stack.o
gcc-11 -c hashtable.c -o hashtable.o
gcc-11 -c parser.c -o parser.o
gcc-11 driver.c lexer.o stack.o hashtable.o parser.o -o exe
./parser