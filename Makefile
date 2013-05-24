all: main

clean:
	$(RM) *.o main

AFRL=/home/kdienes/project/AFRL/AFRL

main: main.o PSMoveController.o PSMoveManager.o
	cc -L/opt/oblong/g-speak3.9/lib -Wl,-rpath,/opt/oblong/g-speak3.9/lib -g -O0 -o $@ $^ -lpsmoveapi -lpsmoveapi_tracker -lm -lc -lstdc++ -lLoam++ -lPlasma++ -lPlasma

CPPFLAGS = -I/opt/oblong/g-speak3.9/include -I${AFRL}/psmoveapi/src -I${AFRL}/psmoveapi/include

CXXFLAGS = -g -O0 -std=c++11 $(CPPFLAGS)
CFLAGS = -g -O0 $(CPPFLAGS)
