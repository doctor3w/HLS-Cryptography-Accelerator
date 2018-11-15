CFLAGS = -Wall -Werror -O3 -pthread

all: aes

aes: aes.cpp
	$(CC) $(CFLAGS) $? -o $@

clean:
	rm -rf aes
