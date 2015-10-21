# makefile for my1codeapp

CC = gcc
CPP = g++

DELETE = rm -rf

CFLAGS += -Wall
LFLAGS += $(LDFLAGS)
OFLAGS +=

# CFLAGS += -DMY1_DEBUG

%: src/%.c src/%.h
	$(CC) $(CFLAGS) -o $@ $< $(LFLAGS) $(OFLAGS)

%: src/%.c
	$(CC) $(CFLAGS) -o $@ $< $(LFLAGS) $(OFLAGS)

%: src/%.cpp src/%.hpp
	$(CPP) $(CFLAGS) -o $@ $< $(LFLAGS) $(OFLAGS)

%: src/%.cpp
	$(CPP) $(CFLAGS) -o $@ $< $(LFLAGS) $(OFLAGS)

# nothing to 'delete' actually
clean:
	-$(DELETE) *.o
