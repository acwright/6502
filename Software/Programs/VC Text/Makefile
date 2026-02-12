make:
	ca65 -v -l ./build/VC\ Text.list -o ./build/VC\ Text.o ./src/VC\ Text.asm
	cl65 -v -C ../../6502.cfg -o ./build/VC\ Text ./build/VC\ Text.o
	
view:
	hexdump -C ./build/VC\ Text.prg

woz:
	bin2woz -a 0x0800 ./build/VC\ Text.prg > ./build/VC\ Text.woz
