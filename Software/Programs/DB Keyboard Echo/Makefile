make:
	ca65 -v -l ./build/DB\ Keyboard\ Echo.list -o ./build/DB\ Keyboard\ Echo.o ./src/*.asm
	cl65 -v -C ../../6502.cfg -o ./build/DB\ Keyboard\ Echo ./build/DB\ Keyboard\ Echo.o
	rm ./build/DB\ Keyboard\ Echo.bin

view:
	hexdump -C ./build/DB\ Keyboard\ Echo.prg

woz:
	bin2woz -a 0x0800 ./build/DB\ Keyboard\ Echo.prg > ./build//DB\ Keyboard\ Echo.woz
