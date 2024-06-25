make:
	ca65 -v -l ./build/SC\ Transmit.list -o ./build/SC\ Transmit.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/SC\ Transmit.bin ./build/SC\ Transmit.o

view:
	hexdump -C ./build/SC\ Transmit.bin

eeprom:
	minipro -p AT28C256	-w ./build/SC\ Transmit.bin