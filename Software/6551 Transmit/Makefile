make:
	ca65 -v -l ./build/6551\ Transmit.list -o ./build/6551\ Transmit.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/6551\ Transmit.bin ./build/6551\ Transmit.o

view:
	hexdump -C ./build/6551\ Transmit.bin