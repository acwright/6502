make:
	ca65 -v -l ./build/SC\ Transmit.list -o ./build/SC\ Transmit.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/SC\ Transmit.prg ./build/SC\ Transmit.o

view:
	hexdump -C ./build/SC\ Transmit.prg

woz:
	hexdump -e '"08%02_ax: " 16/1 "%02X " "\n"' ./build/SC\ Transmit.prg | awk '{print toupper($0)}' > ./build/SC\ Transmit.woz
