make:
	ca65 -v -l ./build/Hello\ World\ LCD.list -o ./build/Hello\ World\ LCD.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/Hello\ World\ LCD.bin ./build/Hello\ World\ LCD.o

view:
	hexdump -C ./build/Hello\ World\ LCD.bin

eeprom:
	minipro -p AT28C256	-w ./build/Hello\ World\ LCD.bin