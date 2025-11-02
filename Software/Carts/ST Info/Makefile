make:
	ca65 -v -l ./build/ST\ Info.list -o ./build/ST\ Info.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/ST\ Info.bin ./build/ST\ Info.o

view:
	hexdump -C ./build/ST\ Info.bin

eeprom:
	minipro -p AT28C256	-w ./build/ST\ Info.bin