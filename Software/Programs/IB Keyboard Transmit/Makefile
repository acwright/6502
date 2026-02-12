make:
	ca65 -v -l ./build/IB\ Keyboard\ Transmit.list -o ./build/IB\ Keyboard\ Transmit.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/IB\ Keyboard\ Transmit.bin ./build/IB\ Keyboard\ Transmit.o

view:
	hexdump -C ./build/IB\ Keyboard\ Transmit.bin

eeprom:
	minipro -p AT28C256	-w ./build/IB\ Keyboard\ Transmit.bin