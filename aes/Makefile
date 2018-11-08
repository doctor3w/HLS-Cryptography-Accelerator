CFLAGS = -Wall -Werror -O3 -pthread

all: aes

aes: aes.c
	$(CC) $(CFLAGS) $? -o $@

clean:
	rm -rf aes
