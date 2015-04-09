main.o : main.c cache.c
	gcc -Wall -o main.o main.c cache.c LRU.c -lm

clean:
	rm main.o
	gcc -Wall -o main.o main.c cache.c LRU.c -lm