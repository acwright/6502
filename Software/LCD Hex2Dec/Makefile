make:
	ca65 -v -l ./build/LCD\ Hex2Dec.list -o ./build/LCD\ Hex2Dec.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/LCD\ Hex2Dec.bin ./build/LCD\ Hex2Dec.o

view:
	hexdump -C ./build/LCD\ Hex2Dec.bin

eeprom:
	minipro -p AT28C256	-w ./build/LCD\ Hex2Dec.bin