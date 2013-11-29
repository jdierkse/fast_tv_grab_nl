CC := g++
CFLAGS := 
LFLAGS := -lboost_program_options -lboost_regex -lboost_system -lboost_filesystem -lboost_serialization -lboost_date_time

ifeq ($(BUILD),debug)
# Debug flags
CFLAGS += -O0 -g
endif

ifeq ($(BUILD),release)
# Optimization flags valid for Intel Atom (DS1813+)
CFLAGS += -march=prescott -mtune=pentium -mfpmath=sse -O3 -s -DNDEBUG 
endif

%.o: %.cpp
	$(CC) -c $(CFLAGS) $< -o $@

fast_tv_grab_nl: main.cpp httpdata.o channel.o channels.o config.o program.o programs.o scanconfig.o tvgids_nl.o functions.o
	g++ -o fast_tv_grab_nl main.cpp httpdata.o channel.o channels.o config.o program.o programs.o scanconfig.o tvgids_nl.o functions.o `curl-config --libs` $(LFLAGS) $(CFLAGS)

httpdata.o: httpdata.cpp httpdata.h

channel.o: channel.cpp channel.h functions.h

channels.o: channels.cpp channels.h channel.h

config.o: config.cpp config.h functions.h

program.o: program.cpp program.h channel.h functions.h

programs.o: programs.cpp programs.h program.h

scanconfig.o: scanconfig.cpp scanconfig.h

tvgids_nl.o: tvgids_nl.cpp tvgids_nl.h provider.h scanconfig.h channels.h programs.h

functions.o: functions.cpp functions.h channels.h programs.h

all: fast_tv_grab_nl

debug:
	make "BUILD=debug"

release:
	make "BUILD=release"

clean:
	-rm *.o fast_tv_grab_nl

install: fast_tv_grab_nl
	cp fast_tv_grab_nl /usr/bin
