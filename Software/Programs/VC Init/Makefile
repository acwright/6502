make:
	ca65 -v -l ./build/VC\ Init.list -o ./build/VC\ Init.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/VC\ Init.prg ./build/VC\ Init.o

view:
	hexdump -C ./build/VC\ Init.prg

woz:
	hexdump -e '"08%02_ax: " 16/1 "%02X " "\n"' ./build/VC\ Init.prg | awk '{print toupper($0)}' > ./build/VC\ Init.woz
