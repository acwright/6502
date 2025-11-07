make:
	ca65 -v -l ./build/SC\ Echo\ Flow\ Control.list -o ./build/SC\ Echo\ Flow\ Control.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/SC\ Echo\ Flow\ Control.prg ./build/SC\ Echo\ Flow\ Control.o

view:
	hexdump -C ./build/SC\ Echo\ Flow\ Control.prg

woz:
	hexdump -e '"08%02_ax: " 16/1 "%02X " "\n"' ./build/SC\ Echo\ Flow\ Control.prg | awk '{print toupper($0)}' > ./build/SC\ Echo\ Flow\ Control.woz
