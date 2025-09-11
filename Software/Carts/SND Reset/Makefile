make:
	ca65 -v -l ./build/SND\ Reset.list -o ./build/SND\ Reset.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/SND\ Reset.bin ./build/SND\ Reset.o

view:
	hexdump -C ./build/SND\ Reset.bin

eeprom:
	minipro -p AT28C256	-w ./build/SND\ Reset.bin