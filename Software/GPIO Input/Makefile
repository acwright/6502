make:
	ca65 -v -l ./build/GPIO\ Input.list -o ./build/GPIO\ Input.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/GPIO\ Input.bin ./build/GPIO\ Input.o

view:
	hexdump -C ./build/GPIO\ Input.bin

eeprom:
	minipro -p AT28C256	-w ./build/GPIO\ Input.bin