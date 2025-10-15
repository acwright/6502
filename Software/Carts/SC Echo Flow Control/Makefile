make:
	ca65 -v -l ./build/SC\ Echo\ Flow\ Control.list -o ./build/SC\ Echo\ Flow\ Control.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/SC\ Echo\ Flow\ Control.bin ./build/SC\ Echo\ Flow\ Control.o

view:
	hexdump -C ./build/SC\ Echo\ Flow\ Control.bin

eeprom:
	minipro -p AT28C256	-w ./build/SC\ Echo\ Flow\ Control.bin