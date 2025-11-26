make:
	ca65 -v -l ./build/LCD\ Clear.list -o ./build/LCD\ Clear.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/LCD\ Clear.prg ./build/LCD\ Clear.o

view:
	hexdump -C ./build/LCD\ Clear.prg

woz:
	bin2woz -a 0x0800 ./build/LCD\ Clear.prg > ./build/LCD\ Clear.woz
