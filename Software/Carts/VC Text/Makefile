make:
	ca65 -v -l ./build/VC\ Text.list -o ./build/VC\ Text.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/VC\ Text.bin ./build/VC\ Text.o

view:
	hexdump -C ./build/VC\ Text.bin

eeprom:
	minipro -p AT28C256	-w ./build/VC\ Text.bin