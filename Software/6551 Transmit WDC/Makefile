make:
	ca65 -v -l ./build/6551\ Transmit\ WDC.list -o ./build/6551\ Transmit\ WDC.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/6551\ Transmit\ WDC.bin ./build/6551\ Transmit\ WDC.o

view:
	hexdump -C ./build/6551\ Transmit\ WDC.bin