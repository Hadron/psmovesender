all: main

clean:
	$(RM) *.o main

AFRL=..

main: main.o PSMoveController.o PSMoveManager.o
	cc -L/opt/oblong/g-speak3.9/lib -Wl,-rpath,/opt/oblong/g-speak3.9/lib -g -O3 -o $@ $^ -lpsmoveapi -lpsmoveapi_tracker -lm -lc -lstdc++ -lLoam++ -lPlasma++ -lPlasma -lLoam

CPPFLAGS = -I/opt/oblong/g-speak3.9/include -I${AFRL}/psmoveapi/external/hidapi/hidapi -I${AFRL}/psmoveapi/src -I${AFRL}/psmoveapi/include

CXXFLAGS = -g -O3 -std=c++11 $(CPPFLAGS)
CFLAGS = -g -O3 $(CPPFLAGS)
