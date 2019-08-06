CC = gcc
CFLAGS = -g -Wall -fPIC -lm -std=gnu99 -O3 -march=native -DCONJUGRAD_FLOAT=64

all: p2c.so

m.PHONY : clean

p2c.so: libc.o
	gcc -shared -Wl,-soname,$@ -o $@ $^

%.o: %.c
	$(CC) -c $(CFLAGS) $<

clean:
	rm -vf p2c.so *.o
