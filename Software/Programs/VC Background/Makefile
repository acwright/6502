make:
	ca65 -v -l ./build/VC\ Background.list -o ./build/VC\ Background.o ./src/*.asm
	cl65 -v -C ../../6502.cfg -o ./build/VC\ Background ./build/VC\ Background.o
	rm ./build/VC\ Background.bin

view:
	hexdump -C ./build/VC\ Background.prg

woz:
	bin2woz -a 0x0800 ./build/VC\ Background.prg > ./build/VC\ Background.woz
