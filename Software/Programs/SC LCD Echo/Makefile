make:
	ca65 -v -l ./build/SC\ LCD\ Echo.list -o ./build/SC\ LCD\ Echo.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/SC\ LCD\ Echo.prg ./build/SC\ LCD\ Echo.o

view:
	hexdump -C ./build/SC\ LCD\ Echo.prg

woz:
	hexdump -e '"08%02_ax: " 16/1 "%02X " "\n"' ./build/SC\ LCD\ Echo.prg | awk '{print toupper($0)}' > ./build/SC\ LCD\ Echo.woz
