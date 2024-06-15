make:
	ca65 -v -l ./build/6551\ Echo\ WDC.list -o ./build/6551\ Echo\ WDC.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/6551\ Echo\ WDC.bin ./build/6551\ Echo\ WDC.o

view:
	hexdump -C ./build/6551\ Echo\ WDC.bin

eeprom:
	minipro -p AT28C256	-w ./build/6551\ Echo\ WDC.bin