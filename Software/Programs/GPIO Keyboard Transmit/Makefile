make:
	ca65 -v -l ./build/PS2\ Keyboard\ Transmit.list -o ./build/PS2\ Keyboard\ Transmit.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/PS2\ Keyboard\ Transmit.prg ./build/PS2\ Keyboard\ Transmit.o

view:
	hexdump -C ./build/PS2\ Keyboard\ Transmit.prg

woz:
	bin2woz -a 0x0800 ./build/PS2\ Keyboard\ Transmit.prg > ./build/PS2\ Keyboard\ Transmit.woz
