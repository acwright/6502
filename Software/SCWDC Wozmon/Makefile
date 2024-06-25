make:
	ca65 -v -l ./build/SCWDC\ Wozmon.list -o ./build/SCWDC\ Wozmon.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/SCWDC\ Wozmon.bin ./build/SCWDC\ Wozmon.o

view:
	hexdump -C ./build/SCWDC\ Wozmon.bin

eeprom:
	minipro -p AT28C256	-w ./build/SCWDC\ Wozmon.bin