
sha256_asm:
	yasm -f x64 -f elf64 -X gnu -g dwarf2 -D LINUX -o sha256_avx1.o sha256_avx1.asm
	yasm -f x64 -f elf64 -X gnu -g dwarf2 -D LINUX -o sha256_avx2_rorx2.o sha256_avx2_rorx2.asm
	yasm -f x64 -f elf64 -X gnu -g dwarf2 -D LINUX -o sha256_avx2_rorx8.o sha256_avx2_rorx8.asm
	yasm -f x64 -f elf64 -X gnu -g dwarf2 -D LINUX -o sha256_sse4.o sha256_sse4.asm
