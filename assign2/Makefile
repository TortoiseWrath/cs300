OOPTS = -Wall -Wextra -g -c -std=c99
LOPTS = -Wall -Wextra -g -std=c99 -pthread

all : p420 p421

p320.o : p320.c
	gcc $(OOPTS) p320.c

p420 : p420.c p320.o
	gcc $(LOPTS) p320.o p420.c -o p420

p421 : p421.c p320.o
	gcc $(LOPTS) p320.o p421.c -o p421

clean    :
	rm -f p320.o p420 p421
