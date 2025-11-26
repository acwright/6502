make:
	ca65 -v -l ./build/GPIO\ Blink.list -o ./build/GPIO\ Blink.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/GPIO\ Blink.prg ./build/GPIO\ Blink.o

view:
	hexdump -C ./build/GPIO\ Blink.prg

woz:
	bin2woz -a 0x0800 ./build/GPIO\ Blink.prg > ./build/GPIO\ Blink.woz
