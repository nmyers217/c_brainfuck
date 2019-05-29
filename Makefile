CC = gcc
OUT = bf
OPTS = -Wall

all:
	$(CC) src/bf.c -o $(OUT) $(OPTS)

debug:
	$(CC) -g src/bf.c -o $(OUT) $(OPTS)

clean:
	rm $(OUT)
