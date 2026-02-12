make:
	ca65 -v -l ./build/IB\ Mouse\ Transmit.list -o ./build/IB\ Mouse\ Transmit.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/IB\ Mouse\ Transmit.bin ./build/IB\ Mouse\ Transmit.o

view:
	hexdump -C ./build/IB\ Mouse\ Transmit.bin

eeprom:
	minipro -p AT28C256	-w ./build/IB\ Mouse\ Transmit.bin