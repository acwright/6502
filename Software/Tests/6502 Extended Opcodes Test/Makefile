make:
	ca65 -v -l ./build/6502\ Extended\ Opcodes\ Test.list -o ./build/6502\ Extended\ Opcodes\ Test.o ./src/*.asm
	cl65 -v -C 6502\ Extended\ Opcodes\ Test.cfg -o ./build/6502\ Extended\ Opcodes\ Test.bin ./build/6502\ Extended\ Opcodes\ Test.o

view:
	hexdump -C ./build/6502\ Extended\ Opcodes\ Test.bin