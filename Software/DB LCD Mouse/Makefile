make:
	ca65 -v -l ./build/DB\ LCD\ Mouse.list -o ./build/DB\ LCD\ Mouse.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/DB\ LCD\ Mouse.bin ./build/DB\ LCD\ Mouse.o

view:
	hexdump -C ./build/DB\ LCD\ Mouse.bin

eeprom:
	minipro -p AT28C256	-w ./build/DB\ LCD\ Mouse.bin