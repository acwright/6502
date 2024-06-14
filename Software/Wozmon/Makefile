make:
	ca65 -v -l ./build/Wozmon.list -o ./build/Wozmon.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/Wozmon.bin ./build/Wozmon.o

view:
	hexdump -C ./build/Wozmon.bin

eeprom:
	minipro -p AT28C256	-w ./build/Wozmon.bin