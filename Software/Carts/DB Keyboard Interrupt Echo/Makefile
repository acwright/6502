make:
	ca65 -v -l ./build/DB\ Keyboard\ Interrupt\ Echo.list -o ./build/DB\ Keyboard\ Interrupt\ Echo.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/DB\ Keyboard\ Interrupt\ Echo.bin ./build/DB\ Keyboard\ Interrupt\ Echo.o

view:
	hexdump -C ./build/DB\ Keyboard\ Interrupt\ Echo.bin

eeprom:
	minipro -p AT28C256	-w ./build/DB\ Keyboard\ Interrupt\ Echo.bin