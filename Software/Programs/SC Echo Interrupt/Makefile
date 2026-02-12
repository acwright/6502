make:
	ca65 -v -l ./build/SC\ Echo\ Interrupt.list -o ./build/SC\ Echo\ Interrupt.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/SC\ Echo\ Interrupt.bin ./build/SC\ Echo\ Interrupt.o

view:
	hexdump -C ./build/SC\ Echo\ Interrupt.bin

eeprom:
	minipro -p AT28C256	-w ./build/SC\ Echo\ Interrupt.bin