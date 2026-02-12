make:
	ca65 -v -l ./build/GPIO\ Input.list -o ./build/GPIO\ Input.o ./src/GPIO\ Input.asm
	cl65 -v -C ../../6502.cfg -o ./build/GPIO\ Input ./build/GPIO\ Input.o
	
view:
	hexdump -C ./build/GPIO\ Input.prg

woz:
	bin2woz -a 0x0800 ./build/GPIO\ Input.prg > ./build/GPIO\ Input.woz
