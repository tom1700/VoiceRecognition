.PHONY:all
.PHONY:clean

all: main clean

main: main.c network.o
	gcc main.c network.o -o main -L. libportaudio.dll.a

network.o: network.c
	gcc -c network.c

clean:
	rm -f *.o