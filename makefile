CC = gcc
CFLAGS = -I.

# Define your source files
SRCS = driver.c parser.c lexer.c stack.c hashtable.c

# Define the object files by replacing the .c extension with .o
OBJS = $(SRCS:.c=.o)

# Define the dependencies for the target 'all'
# 'all' depends on the object files
makefile: $(OBJS)
	$(CC) $(CFLAGS) -o stage1exe $(OBJS)

# Define the rule to generate .o files from .c files
# Each .c file generates a .o file
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Define the clean rule to remove the executable and object files
clean:
	rm -f myprogram $(OBJS)
