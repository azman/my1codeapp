# makefile for single file app (my1codeapp)

ALLAPP = $(subst .c,,$(subst src/,,$(wildcard src/*.c)))
ALLAPP += $(subst .c,,$(wildcard *.c))

TARGET ?=
TGTLBL = app

CC = gcc

DELETE = rm -rf

CFLAGS += -Wall -static
LFLAGS += $(LDFLAGS)
OFLAGS +=
XFLAGS += -D_LARGEFILE_SOURCE=1 -D_FILE_OFFSET_BITS=64

.PHONY: dummy

debug: CFLAGS += -DMY1_DEBUG

$(TARGET): src/$(TARGET).c
	$(CC) $(CFLAGS) -o $(TGTLBL) $< $(LFLAGS) $(OFLAGS)

dummy:
	@echo "Run 'make <app>' or 'make TARGET=<app>'"
	@echo "  <app> = { $(ALLAPP) }"

debug: $(TARGET)

%: src/%.c src/%.h
	$(CC) $(CFLAGS) -o $@ $< $(LFLAGS) $(OFLAGS)

%: src/%.c
	$(CC) $(CFLAGS) -o $@ $< $(LFLAGS) $(OFLAGS)

%: %.c %.h
	$(CC) $(CFLAGS) -o $@ $< $(LFLAGS) $(OFLAGS)

%: %.c
	$(CC) $(CFLAGS) -o $@ $< $(LFLAGS) $(OFLAGS)

clean:
	-$(DELETE) $(ALLAPP) $(TGTLBL)
