CC=gcc
CFLAGS=-Wall -g
LIBS=
DEPS=
OBJS=main.o
EXE=jshell

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(EXE): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

clean: 
	$(RM) $(OBJS) $(EXE)

bgTest: bgTest.c
	$(CC) -o $@ $^ $(CFLAGS)
