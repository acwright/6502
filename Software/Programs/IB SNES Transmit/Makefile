make:
	ca65 -v -l ./build/IB\ SNES\ Transmit.list -o ./build/IB\ SNES\ Transmit.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/IB\ SNES\ Transmit.bin ./build/IB\ SNES\ Transmit.o

view:
	hexdump -C ./build/IB\ SNES\ Transmit.bin

eeprom:
	minipro -p AT28C256	-w ./build/IB\ SNES\ Transmit.bin