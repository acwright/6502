make:
	ca65 -v -l ./build/LCD\ Hello\ World.list -o ./build/LCD\ Hello\ World.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/LCD\ Hello\ World.bin ./build/LCD\ Hello\ World.o

view:
	hexdump -C ./build/LCD\ Hello\ World.bin

eeprom:
	minipro -p AT28C256	-w ./build/LCD\ Hello\ World.bin