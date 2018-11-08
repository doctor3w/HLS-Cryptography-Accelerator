src = $(wildcard src/*.cpp)
obj = $(src:.cpp=.o)

LDFLAGS =

sha256: $(obj) sha256_asm
	$(CC) -o $@ $^ $(LDFLAGS)

.PHONY: clean
	rm -f $(obj) sha256


.PHONY: sha256_asm
sha256_asm:
	$(MAKE) -C sha256_code_release/
