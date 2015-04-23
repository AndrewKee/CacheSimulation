main.o : main.c cache.c LRU.c
	gcc -std=c99 -Wall -O3 -o main.o main.c cache.c LRU.c -lm

clean:
	rm main.o
	gcc -std=c99 -Wall -O3 -o main.o main.c cache.c LRU.c -lm