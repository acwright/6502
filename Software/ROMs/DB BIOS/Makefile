make:
	ca65 -v -l ./build/DB\ BIOS.list -o ./build/DB\ BIOS.o ./src/DB\ BIOS.asm
	cl65 -v -C 6502.cfg -o ./build/DB\ BIOS.bin ./build/DB\ BIOS.o

view:
	hexdump -C ./build/DB\ BIOS.bin

eeprom:
	minipro -p AT28C256	-w ./build/DB\ BIOS.bin