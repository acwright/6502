make:
	ca65 -v -l ./build/Keyboard\ Polling.list -o ./build/Keyboard\ Polling.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/Keyboard\ Polling.bin ./build/Keyboard\ Polling.o

view:
	hexdump -C ./build/Keyboard\ Polling.bin

eeprom:
	minipro -p AT28C256	-w ./build/Keyboard\ Polling.bin