make:
	ca65 -v -l ./build/DB\ Mouse\ Info.list -o ./build/DB\ Mouse\ Info.o ./src/DB\ Mouse\ Info.asm
	cl65 -v -C ../../6502.cfg -o ./build/DB\ Mouse\ Info ./build/DB\ Mouse\ Info.o
	
view:
	hexdump -C ./build/DB\ Mouse\ Info.prg

woz:
	bin2woz -a 0x0800 ./build/DB\ Mouse\ Info.prg > ./build//DB\ Mouse\ Info.woz
