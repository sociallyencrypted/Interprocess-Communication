all: socket fifo shared
socket: src/P1socket.c src/P2socket.c
	gcc -o bin/P1socket src/P1socket.c
	gcc -o bin/P2socket src/P2socket.c
fifo: src/P1fifo.c src/P2fifo.c
	gcc -o bin/P1fifo src/P1fifo.c
	gcc -o bin/P2fifo src/P2fifo.c
shared: src/P1shared.c src/P2shared.c
	gcc -o bin/P1shared src/P1shared.c
	gcc -o bin/P2shared src/P2shared.c
clean:
	rm -f bin/*