make:
	ca65 -v -l ./build/ST\ Info.list -o ./build/ST\ Info.o ./src/*.asm
	cl65 -v -C ../../6502.cfg -o ./build/ST\ Info ./build/ST\ Info.o
	rm ./build/ST\ Info.bin
	
view:
	hexdump -C ./build/ST\ Info.prg

woz:
	bin2woz -a 0x0800 ./build/ST\ Info.prg > ./build/ST\ Info.woz
