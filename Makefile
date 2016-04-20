CFLAGS=-O3 -std=c++0x -fpermissive -w
all: a.out

a.out: main.o
	g++ ${CFLAGS} $^ -lsfml-system -lsfml-graphics -lsfml-window -lsfml-audio -o $@

%.o: %.cc
	g++ ${CFLAGS} -c $^ -o $@
