make:
	ca65 -v -l ./build/SC\ Echo.list -o ./build/SC\ Echo.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/SC\ Echo.prg ./build/SC\ Echo.o

view:
	hexdump -C ./build/SC\ Echo.prg

woz:
	hexdump -e '"08%02_ax: " 16/1 "%02X " "\n"' ./build/SC\ Echo.prg | awk '{print toupper($0)}' > ./build/SC\ Echo.woz
