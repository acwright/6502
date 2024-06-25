make:
	ca65 -v -l ./build/GPIO\ Blink.list -o ./build/GPIO\ Blink.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/GPIO\ Blink.bin ./build/GPIO\ Blink.o

view:
	hexdump -C ./build/GPIO\ Blink.bin

eeprom:
	minipro -p AT28C256	-w ./build/GPIO\ Blink.bin