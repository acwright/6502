make:
	ca65 -v -l ./build/GPIO\ Keyboard\ Transmit.list -o ./build/GPIO\ Keyboard\ Transmit.o ./src/*.asm
	cl65 -v -C ../../6502.cfg -o ./build/GPIO\ Keyboard\ Transmit ./build/GPIO\ Keyboard\ Transmit.o
	rm ./build/GPIO\ Keyboard\ Transmit.bin
	
view:
	hexdump -C ./build/GPIO\ Keyboard\ Transmit.prg

woz:
	bin2woz -a 0x0800 ./build/GPIO\ Keyboard\ Transmit.prg > ./build/GPIO\ Keyboard\ Transmit.woz
