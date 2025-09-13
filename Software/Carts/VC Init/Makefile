make:
	ca65 -v -l ./build/VC\ Init.list -o ./build/VC\ Init.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/VC\ Init.bin ./build/VC\ Init.o

view:
	hexdump -C ./build/VC\ Init.bin

eeprom:
	minipro -p AT28C256	-w ./build/VC\ Init.bin