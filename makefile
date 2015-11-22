# makefile for my1codeapp

ALLAPP = $(subst .c,,$(subst src/,,$(wildcard src/*.c)))
TARGET ?=
TGTLBL = app

CC = gcc
CPP = g++

DELETE = rm -rf

CFLAGS += -Wall
LFLAGS += $(LDFLAGS)
OFLAGS +=

.PHONY: dummy

debug: CFLAGS += -DMY1_DEBUG

$(TARGET): src/$(TARGET).c
	$(CC) $(CFLAGS) -o $(TGTLBL) $< $(LFLAGS) $(OFLAGS)

dummy:
	@echo "Run 'make <app>' or 'make TARGET=<app>'"
	@echo "  <app> = { $(ALLAPP) }"

debug: $(TARGET)

%: src/%.c
	$(CC) $(CFLAGS) -o $@ $< $(LFLAGS) $(OFLAGS)

%: src/%.cpp
	$(CPP) $(CFLAGS) -o $@ $< $(LFLAGS) $(OFLAGS)

# nothing to 'delete' actually
clean:
	-$(DELETE) $(ALLAPP) $(TGTLBL)
