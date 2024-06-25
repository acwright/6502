make:
	ca65 -v -l ./build/DB\ Wozmon.list -o ./build/DB\ Wozmon.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/DB\ Wozmon.bin ./build/DB\ Wozmon.o

view:
	hexdump -C ./build/DB\ Wozmon.bin

eeprom:
	minipro -p AT28C256	-w ./build/DB\ Wozmon.bin