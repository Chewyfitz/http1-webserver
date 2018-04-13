CC=gcc
CFLAGS=
DEPS=
OBJ= server.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

server: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f server *.o