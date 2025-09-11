make:
	ca65 -v -l ./build/SC\ Wozmon.list -o ./build/SC\ Wozmon.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/SC\ Wozmon.bin ./build/SC\ Wozmon.o

view:
	hexdump -C ./build/SC\ Wozmon.bin

eeprom:
	minipro -p AT28C256	-w ./build/SC\ Wozmon.bin