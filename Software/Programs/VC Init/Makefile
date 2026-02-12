make:
	ca65 -v -l ./build/VC\ Init.list -o ./build/VC\ Init.o ./src/VC\ Init.asm
	cl65 -v -C ../../6502.cfg -o ./build/VC\ Init ./build/VC\ Init.o
	
view:
	hexdump -C ./build/VC\ Init.prg

woz:
	bin2woz -a 0x0800 ./build/VC\ Init.prg > ./build/VC\ Init.woz
