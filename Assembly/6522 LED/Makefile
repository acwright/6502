make:
	ca65 -v -l ./build/6522\ LED.list -o ./build/6522\ LED.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/6522\ LED.bin ./build/6522\ LED.o

view:
	hexdump -C ./build/6522\ LED.bin