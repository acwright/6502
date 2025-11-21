make:
	ca65 -v -l ./build/PS2\ Keyboard\ Transmit.list -o ./build/PS2\ Keyboard\ Transmit.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/PS2\ Keyboard\ Transmit.prg ./build/PS2\ Keyboard\ Transmit.o

view:
	hexdump -C ./build/PS2\ Keyboard\ Transmit.prg

woz:
	hexdump -e '"08%02_ax: " 16/1 "%02X " "\n"' ./build/PS2\ Keyboard\ Transmit.prg | awk '{print toupper($0)}' > ./build/PS2\ Keyboard\ Transmit.woz
