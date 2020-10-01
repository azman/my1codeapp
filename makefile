# makefile for single file app (my1codeapp)

ALLAPP = $(subst .c,,$(subst src/,,$(wildcard src/*.c)))
ALLAPP += $(subst .cpp,,$(subst src/,,$(wildcard src/*.cpp)))
ALLAPP += $(subst .c,,$(wildcard *.c))

TARGET ?=
TGTLBL = app
OBJLST ?=
DOLINK ?=
DOFLAG ?=

CC = gcc
CP = g++

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

# TARGET can be temporary code (reside at top level)
$(TARGET): $(OBJLST) $(TARGET).o
	$(CC) $(CFLAGS) -o $(TGTLBL) $^ $(LFLAGS) $(OFLAGS)

dummy:
	@echo "Run 'make <app>' or 'make TARGET=<app>'"
	@echo "  <app> = { $(ALLAPP) }"
	@echo
	@echo "To link a library (e.g. math), do 'make <app> DOLINK=-lm'"

# make TARGET=uartsend OBJLST="my1keys.o my1uart.o my1bytes.o"
uart-send: CFLAGS += -DPROGNAME=\"uart-send\"
uart-send: my1keys.o my1uart.o my1bytes.o uartsend.o
	$(CC) $(CFLAGS) -o $@ $^ $(LFLAGS) $(OFLAGS)

%: src/%.c
	$(CC) $(CFLAGS) -o $@ $< $(LFLAGS) $(OFLAGS)

# added c++ support...
%: src/%.cpp
	$(CP) $(CFLAGS) -o $@ $< $(LFLAGS) $(OFLAGS)

# for temporary quick testing of c source code
%: %.c
	$(CC) $(CFLAGS) -o $@ $< $(LFLAGS) $(OFLAGS)

# ...or c++!
%: %.cpp
	$(CP) $(CFLAGS) -o $@ $< $(LFLAGS) $(OFLAGS)

# to compile those external code(s) => OBJLST!
%.o: $(EXTPATH)/%.c $(EXTPATH)/%.h
	$(CC) -c $(CFLAGS) -o $@ $<

# for fancy TARGETs
%.o: src/%.c
	$(CC) -c $(CFLAGS) -o $@ $<

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	-$(DELETE) $(ALLAPP) $(TGTLBL) uart-send *.o
