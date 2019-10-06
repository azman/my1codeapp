# makefile for single file app (my1codeapp)

ALLAPP = $(subst .c,,$(subst src/,,$(wildcard src/*.c)))
ALLAPP += $(subst .c,,$(wildcard *.c))

TARGET ?=
TGTLBL = app
OBJLST ?=
DOLINK ?=
DOFLAG ?=

CC = gcc

DELETE = rm -rf

CFLAGS += -Wall -static $(DOFLAG)
LFLAGS += $(LDFLAGS)
OFLAGS +=
XFLAGS += -D_LARGEFILE_SOURCE=1 -D_FILE_OFFSET_BITS=64
CFLAGS += $(DOFLAG)
LFLAGS += $(DOLINK)

# i can still squeeze in some external code(s) => OBJLST!
EXTPATH = ../my1codelib/src
CFLAGS += -I$(EXTPATH)

.PHONY: dummy $(TARGET)

$(TARGET): src/$(TARGET).c $(OBJLST)
	$(CC) $(CFLAGS) -o $(TGTLBL) $^ $(LFLAGS) $(OFLAGS)

dummy:
	@echo "Run 'make <app>' or 'make TARGET=<app>'"
	@echo "  <app> = { $(ALLAPP) }"
	@echo
	@echo "To link a library (e.g. math), do 'make <app> DOLINK=-lm'"

%: src/%.c src/%.h
	$(CC) $(CFLAGS) -o $@ $< $(LFLAGS) $(OFLAGS)

%: src/%.c
	$(CC) $(CFLAGS) -o $@ $< $(LFLAGS) $(OFLAGS)

# for temporary quick testing of c source code
%: %.c
	$(CC) $(CFLAGS) -o $@ $< $(LFLAGS) $(OFLAGS)

# to compile those external code(s) => OBJLST!
%.o: $(EXTPATH)/%.c $(EXTPATH)/%.h
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	-$(DELETE) $(ALLAPP) $(TGTLBL) *.o
