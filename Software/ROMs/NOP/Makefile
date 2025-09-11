make:
	ca65 -v -l ./build/NOP.list -o ./build/NOP.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/NOP.bin ./build/NOP.o

view:
	hexdump -C ./build/NOP.bin

eeprom:
	minipro -p AT28C256	-w ./build/NOP.bin