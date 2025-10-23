make:
	ca65 -v -l ./build/6502\ Decimal\ Test.list -o ./build/6502\ Decimal\ Test.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/6502\ Decimal\ Test.bin ./build/6502\ Decimal\ Test.o

view:
	hexdump -C ./build/6502\ Decimal\ Test.bin