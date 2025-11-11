make:
	ca65 -v -l ./build/DB\ Keyboard\ Echo.list -o ./build/DB\ Keyboard\ Echo.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/DB\ Keyboard\ Echo.prg ./build/DB\ Keyboard\ Echo.o

view:
	hexdump -C ./build/DB\ Keyboard\ Echo.prg

woz:
	hexdump -e '"08%02_ax: " 16/1 "%02X " "\n"' ./build/DB\ Keyboard\ Echo.prg | awk '{print toupper($0)}' > ./build//DB\ Keyboard\ Echo.woz
