EXE=../bin/ppcemul
LIB=../lib/libppcemul.a
CC=gcc
AR=ar rcvs

##################################################################################################
#                                                                                                #
# CXXFLAGS = flags for the C compiler                                                            #
#                                                                                                #
##################################################################################################

CFLAGS=-O3 -fomit-frame-pointer -fexpensive-optimizations -fstrength-reduce -fschedule-insns2 -funroll-loops
LIBS=-lm
INCLUDES=-I../include

SOURCES= \
./branch.c \
./loadstore.c \
./ppcemul.c \
./floatingpoint.c \
./systemregisters.c \
./debug.c \
./read_elf.c \
./integer.c \
./decode.c \
./memory.c \
./registers.c \
./loader.c \
./fetch.c \
./syscall.c \
./xmlize.c


OBJS=$(SOURCES:.c=.o)
DEPS=$(SOURCES:.c=.d)

all: $(EXE) $(LIB)
	@echo "The PowerPC Emulator has been successfully built"

clean:
	rm -f $(EXE) $(LIB) $(OBJS) $(DEPS) main.d main.o

%.d: %.c
	set -e; $(CC) -M $(CFLAGS) $(INCLUDES) $< \
                  | sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@; \
                [ -s $@ ] || rm -f $@

include $(DEPS)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

include main.d
	
$(EXE): $(OBJS) main.o
	$(CC) $(CFLAGS) $(OBJS) main.o $(LIBS) -o $(EXE)

$(LIB) : $(OBJS)
	$(AR) $(LIB) $(OBJS)
