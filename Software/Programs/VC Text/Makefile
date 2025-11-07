make:
	ca65 -v -l ./build/VC\ Text.list -o ./build/VC\ Text.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/VC\ Text.prg ./build/VC\ Text.o

view:
	hexdump -C ./build/VC\ Text.prg

woz:
	hexdump -e '"08%02_ax: " 16/1 "%02X " "\n"' ./build/VC\ Text.prg | awk '{print toupper($0)}' > ./build/VC\ Text.woz
