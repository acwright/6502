make:
	ca65 -v -l ./build/SCWDC\ Transmit.list -o ./build/SCWDC\ Transmit.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/SCWDC\ Transmit.bin ./build/SCWDC\ Transmit.o

view:
	hexdump -C ./build/SCWDC\ Transmit.bin

eeprom:
	minipro -p AT28C256	-w ./build/SCWDC\ Transmit.bin