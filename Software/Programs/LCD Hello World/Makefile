make:
	ca65 -v -l ./build/LCD\ Hello\ World.list -o ./build/LCD\ Hello\ World.o ./src/*.asm
	cl65 -v -C ../../6502.cfg -o ./build/LCD\ Hello\ World ./build/LCD\ Hello\ World.o
	rm ./build/LCD\ Hello\ World.bin
	
view:
	hexdump -C ./build/LCD\ Hello\ World.prg

woz:
	bin2woz -a 0x0800 ./build/LCD\ Hello\ World.prg > ./build/LCD\ Hello\ World.woz
