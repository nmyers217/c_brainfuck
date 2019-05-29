CC = gcc
OUT = bf
OPTS = -Wall -O3
DEBUG_OPTS = -Wall

all:
	$(CC) src/bf.c -o $(OUT) $(OPTS)

debug:
	$(CC) -g src/bf.c -o $(OUT) $(DEBUG_OPTS)

clean:
	rm $(OUT)
