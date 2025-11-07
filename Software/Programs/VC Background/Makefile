make:
	ca65 -v -l ./build/VC\ Background.list -o ./build/VC\ Background.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/VC\ Background.prg ./build/VC\ Background.o

view:
	hexdump -C ./build/VC\ Background.prg

woz:
	hexdump -e '"08%02_ax: " 16/1 "%02X " "\n"' ./build/VC\ Background.prg | awk '{print toupper($0)}' > ./build/VC\ Background.woz
