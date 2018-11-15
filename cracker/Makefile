src = $(wildcard *.cc)
obj = $(src:.cpp=.o)


LDFLAGS =

cracker: $(src)
	$(CC) -std=c++0x -o $@ $^ $(LDFLAGS)

.PHONY: clean
	rm -f $(obj) cracker

