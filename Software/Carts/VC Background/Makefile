make:
	ca65 -v -l ./build/VC\ Background.list -o ./build/VC\ Background.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/VC\ Background.bin ./build/VC\ Background.o

view:
	hexdump -C ./build/VC\ Background.bin

eeprom:
	minipro -p AT28C256	-w ./build/VC\ Background.bin