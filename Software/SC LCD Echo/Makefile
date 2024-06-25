make:
	ca65 -v -l ./build/SC\ LCD\ Echo.list -o ./build/SC\ LCD\ Echo.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/SC\ LCD\ Echo.bin ./build/SC\ LCD\ Echo.o

view:
	hexdump -C ./build/SC\ LCD\ Echo.bin

eeprom:
	minipro -p AT28C256	-w ./build/SC\ LCD\ Echo.bin