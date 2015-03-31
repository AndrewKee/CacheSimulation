main.o : main.c cache.c
	gcc -Wall -o main main.c cache.c

clean:
	rm main.o 