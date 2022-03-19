CC = clang
CFLAGS = -Wall -Wextra -Werror -Wpedantic

all: encode decode

encode: encode.o node.o io.o code.o huffman.o pq.o stack.o
	$(CC) -o encode encode.o node.o io.o code.o huffman.o pq.o stack.o

decode: decode.o node.o io.o code.o huffman.o pq.o stack.o
	$(CC) -o decode decode.o node.o io.o code.o huffman.o pq.o stack.o

node.o: node.c
	$(CC) $(CFLAGS) -c node.c

pq.o: pq.c
	$(CC) $(CFLAGS) -c pq.c

code.o: code.c
	$(CC) $(CFLAGS) -c code.c

io.o: io.c
	$(CC) $(CFLAGS) -c io.c

stack.o: stack.c
	$(CC) $(CFLAGS) -c stack.c

huffman.o: huffman.c
	$(CC) $(CFLAGS) -c huffman.c

encode.o: encode.c
	$(CC) $(CFLAGS) -c encode.c

decode.o: decode.c
	$(CC) $(CFLAGS) -c decode.c

clean:
	rm -f encode *.o decode *.o huffman *.o stack *.o io *.o code *.o pq *.o node *.o

format:
	clang-format -i -style=file *.[ch]

