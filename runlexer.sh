gcc -c hashtable.c -o hashtable.o
gcc lexer.c hashtable.o -o lexer
./lexer