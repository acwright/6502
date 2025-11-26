make:
	ca65 -v -l ./build/SC\ Echo.list -o ./build/SC\ Echo.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/SC\ Echo.prg ./build/SC\ Echo.o

view:
	hexdump -C ./build/SC\ Echo.prg

woz:
	bin2woz -a 0x0800 ./build/SC\ Echo.prg > ./build/SC\ Echo.woz
