make:
	ca65 -v -l ./build/6551\ LCD.list -o ./build/6551\ LCD.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/6551\ LCD.bin ./build/6551\ LCD.o

view:
	hexdump -C ./build/6551\ LCD.bin

eeprom:
	minipro -p AT28C256	-w ./build/6551\ LCD.bin