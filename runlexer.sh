gcc -c lexer.c -o lexer.o
gcc -c stack.c -o stack.o
gcc -c hashtable.c -o hashtable.o
gcc -c parser.c -o parser.o
gcc driver.c lexer.o stack.o hashtable.o parser.o -o exe
./parser