make:
	ca65 -v -l ./build/LCD\ Clear.list -o ./build/LCD\ Clear.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/LCD\ Clear.bin ./build/LCD\ Clear.o

view:
	hexdump -C ./build/LCD\ Clear.bin

eeprom:
	minipro -p AT28C256	-w ./build/LCD\ Clear.bin