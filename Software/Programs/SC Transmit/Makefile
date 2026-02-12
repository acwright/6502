make:
	ca65 -v -l ./build/SC\ Transmit.list -o ./build/SC\ Transmit.o ./src/SC\ Transmit.asm
	cl65 -v -C ../../6502.cfg -o ./build/SC\ Transmit ./build/SC\ Transmit.o
	
view:
	hexdump -C ./build/SC\ Transmit.prg

woz:
	bin2woz -a 0x0800 ./build/SC\ Transmit.prg > ./build/SC\ Transmit.woz
