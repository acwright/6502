make:
	ca65 -v -l ./build/SCWDC\ Echo.list -o ./build/SCWDC\ Echo.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/SCWDC\ Echo.bin ./build/SCWDC\ Echo.o

view:
	hexdump -C ./build/SCWDC\ Echo.bin

eeprom:
	minipro -p AT28C256	-w ./build/SCWDC\ Echo.bin