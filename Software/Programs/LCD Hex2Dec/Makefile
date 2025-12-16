make:
	ca65 -v -l ./build/LCD\ Hex2Dec.list -o ./build/LCD\ Hex2Dec.o ./src/*.asm
	cl65 -v -C ../../6502.cfg -o ./build/LCD\ Hex2Dec ./build/LCD\ Hex2Dec.o
	rm ./build/LCD\ Hex2Dec.bin
	
view:
	hexdump -C ./build/LCD\ Hex2Dec.prg

woz:
	bin2woz -a 0x0800 ./build/LCD\ Hex2Dec.prg > ./build/LCD\ Hex2Dec.woz
