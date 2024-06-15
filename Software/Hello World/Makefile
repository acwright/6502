make:
	ca65 -v -l ./build/Hello\ World.list -o ./build/Hello\ World.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/Hello\ World.bin ./build/Hello\ World.o

view:
	hexdump -C ./build/Hello\ World.bin

eeprom:
	minipro -p AT28C256	-w ./build/Hello\ World.bin