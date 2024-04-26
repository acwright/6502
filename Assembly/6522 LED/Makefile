make:
	ca65 -v -l ./build/6502\ LED.list -o ./build/6502\ LED.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/6502\ LED.bin ./build/6502\ LED.o

view:
	hexdump -C ./build/6502\ LED.bin