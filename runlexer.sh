gcc -c lexer.c -o lexer.o
gcc -c stack.c -o stack.o
gcc -c hashtable.c -o hashtable.o
gcc parser.c stack.o lexer.o hashtable.o -o parser
./parser