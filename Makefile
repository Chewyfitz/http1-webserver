CC=gcc
CFLAGS=
DEPS= process.h
OBJ= server.o process.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

server: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f server *.o