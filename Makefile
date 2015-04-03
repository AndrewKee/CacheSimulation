main.o : main.c cache.c
	gcc -Wall -o main main.c cache.c -lm

clean:
	rm main
	gcc -Wall -o main main.c cache.c -lm