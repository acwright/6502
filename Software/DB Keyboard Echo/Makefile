make:
	ca65 -v -l ./build/DB\ Keyboard\ Echo.list -o ./build/DB\ Keyboard\ Echo.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/DB\ Keyboard\ Echo.bin ./build/DB\ Keyboard\ Echo.o

view:
	hexdump -C ./build/DB\ Keyboard\ Echo.bin