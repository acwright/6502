make:
	ca65 -v -l ./build/Wozmon.list -o ./build/Wozmon.o ./src/Wozmon.asm
	cl65 -v -C Wozmon.cfg -o ./build/Wozmon ./build/Wozmon.o
	
view:
	hexdump -C ./build/Wozmon.bin

eeprom:
	minipro -p AT28C256	-w ./build/Wozmon.bin