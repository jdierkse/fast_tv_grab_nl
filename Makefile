CFLAGS := 
LFLAGS := -lboost_program_options -lboost_regex

ifeq ($(BUILD),debug)   
CFLAGS += -O0 -g
else
# Optimization flags valid for Intel Atom (DS1813+)
CFLAGS += -march=prescott -mtune=pentium -mfpmath=sse -O3 -s -DNDEBUG 
endif

fast_tv_grab_nl: main.cpp httpdata.o channel.o channels.o config.o program.o programs.o functions.o
	g++ -o fast_tv_grab_nl main.cpp httpdata.o channel.o channels.o config.o program.o programs.o functions.o `curl-config --libs` $(LFLAGS) $(CFLAGS) $(ARGS)

httpdata.o: httpdata.cpp httpdata.h
	g++ -c httpdata.cpp $(CFLAGS) $(ARGS)

channel.o: channel.cpp channel.h
	g++ -c channel.cpp $(CFLAGS) $(ARGS)

channels.o: channels.cpp channels.h channel.h
	g++ -c channels.cpp $(CFLAGS) $(ARGS)

config.o: config.cpp config.h functions.h
	g++ -c config.cpp $(CFLAGS) $(ARGS)

program.o: program.cpp program.h channel.h functions.h
	g++ -c program.cpp $(CFLAGS) $(ARGS)

programs.o: programs.cpp programs.h program.h
	g++ -c programs.cpp $(CFLAGS) $(ARGS)

functions.o: functions.cpp functions.h channels.h programs.h
	g++ -c functions.cpp $(CFLAGS) $(ARGS)

all: fast_tv_grab_nl

debug:
	make "BUILD=debug"

clean:
	-rm *.o fast_tv_grab_nl

install: fast_tv_grab_nl
	su -c"cp fast_tv_grab_nl /usr/bin"
