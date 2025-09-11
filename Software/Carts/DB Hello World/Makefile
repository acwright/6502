make:
	ca65 -v -l ./build/DB\ Hello\ World.list -o ./build/DB\ Hello\ World.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/DB\ Hello\ World.bin ./build/DB\ Hello\ World.o

view:
	hexdump -C ./build/DB\ Hello\ World.bin