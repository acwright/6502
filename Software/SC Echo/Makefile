make:
	ca65 -v -l ./build/SC\ Echo.list -o ./build/SC\ Echo.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/SC\ Echo.bin ./build/SC\ Echo.o

view:
	hexdump -C ./build/SC\ Echo.bin

eeprom:
	minipro -p AT28C256	-w ./build/SC\ Echo.bin