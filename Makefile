CC = gcc
OUT = bf
OPTS = -Wall -O3
DEBUG_OPTS = -g -Wall

all:
	$(CC) $(OPTS) src/bf.c -o $(OUT)

debug:
	$(CC) $(DEBUG_OPTS) src/bf.c -o $(OUT)

clean:
	rm $(OUT)
