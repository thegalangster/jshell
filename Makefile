$(CC)=gcc
CFLAGS=-Wall
LIBS=
DEPS=
OBJ=main.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

debug: CC += -g
debug: jshell

jshell: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

