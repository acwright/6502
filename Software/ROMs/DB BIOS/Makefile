make:
	ca65 -v -l ./build/BIOS.list -o ./build/BIOS.o ./src/BIOS.asm
	cl65 -v -C ../../6502.cfg -o ./build/BIOS ./build/BIOS.o
	rm ./build/BIOS.prg

view:
	hexdump -C ./build/BIOS.bin

eeprom:
	minipro -p AT28C256	-w ./build/BIOS.bin