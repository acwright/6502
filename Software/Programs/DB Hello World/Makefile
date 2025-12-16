make:
	ca65 -v -l ./build/DB\ Hello\ World.list -o ./build/DB\ Hello\ World.o ./src/*.asm
	cl65 -v -C ../../6502.cfg -o ./build/DB\ Hello\ World ./build/DB\ Hello\ World.o
	rm ./build/DB\ Hello\ World.bin
	
view:
	hexdump -C ./build/DB\ Hello\ World.prg

woz:
	bin2woz -a 0x0800 ./build/DB\ Hello\ World.prg > ./build//DB\ Hello\ World.woz
