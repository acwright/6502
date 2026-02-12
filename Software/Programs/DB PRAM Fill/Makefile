make:
	ca65 -v -l ./build/DB\ PRAM\ Fill.list -o ./build/DB\ PRAM\ Fill.o ./src/DB\ PRAM\ Fill.asm
	cl65 -v -C ../../6502.cfg -o ./build/DB\ PRAM\ Fill ./build/DB\ PRAM\ Fill.o
	
view:
	hexdump -C ./build/DB\ PRAM\ Fill.prg

woz:
	bin2woz -a 0x0800 ./build/DB\ PRAM\ Fill.prg > ./build//DB\ PRAM\ Fill.woz
