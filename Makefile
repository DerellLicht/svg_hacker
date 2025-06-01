# makefile for media_list app
SHELL=cmd.exe
USE_DEBUG = NO
USE_64BIT = NO

ifeq ($(USE_64BIT),YES)
TOOLS=d:\tdm64\bin
else
TOOLS=c:\tdm32\bin
endif

ifeq ($(USE_DEBUG),YES)
CFLAGS = -Wall -g -c
CxxFLAGS = -Wall -g -c
LFLAGS = -g
else
CFLAGS = -Wall -s -O3 -c
CxxFLAGS = -Wall -s -O3 -c
LFLAGS = -s -O3
endif
CFLAGS += -Weffc++
CFLAGS += -Wno-write-strings
ifeq ($(USE_64BIT),YES)
CFLAGS += -DUSE_64BIT
CxxFLAGS += -DUSE_64BIT
endif

#  needed by qualify.cpp
LIBS=-lshlwapi

BIN = svg_hacker

CPPSRC=svg_hacker.cpp read_svg_file.cpp common.cpp qualify.cpp

OBJS = $(CPPSRC:.cpp=.o)

#  clang-tidy options
CHFLAGS = -header-filter=.*
CHTAIL = -- 
#CHTAIL += -Ider_libs
ifeq ($(USE_64BIT),YES)
CHTAIL += -DUSE_64BIT
endif
ifeq ($(USE_UNICODE),YES)
CHTAIL += -DUNICODE -D_UNICODE
endif

#**************************************************************************
%.o: %.cpp
	$(TOOLS)\g++ $(CFLAGS) $<

all: $(BIN).exe

clean:
	rm -f *.o *.exe *~ *.zip

dist:
	rm -f $(BIN).zip
	zip $(BIN).zip $(BIN) Readme.md

check:
	cmd /C "d:\clang\bin\clang-tidy.exe $(CHFLAGS) $(CPPSRC) $(CHTAIL)"

lint:
	cmd /C "c:\lint9\lint-nt +v -width(160,4) $(LiFLAGS) -ic:\lint9 mingw.lnt -os(_lint.tmp) lintdefs.cpp $(CPPSRC)"

wc:
	wc -l *.cpp

depend: 
	makedepend $(CSRC) $(CPPSRC)

$(BIN).exe: $(OBJS)
	$(TOOLS)\g++ $(OBJS) $(LFLAGS) -o $(BIN) $(LIBS) 

# DO NOT DELETE

svg_hacker.o: common.h svg_hacker.h qualify.h
read_svg_file.o: common.h svg_hacker.h
common.o: common.h
qualify.o: qualify.h
