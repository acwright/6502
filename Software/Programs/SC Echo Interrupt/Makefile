make:
	ca65 -v -l ./build/SC\ Echo\ Interrupt.list -o ./build/SC\ Echo\ Interrupt.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/SC\ Echo\ Interrupt.prg ./build/SC\ Echo\ Interrupt.o

view:
	hexdump -C ./build/SC\ Echo\ Interrupt.prg

woz:
	hexdump -e '"08%02_ax: " 16/1 "%02X " "\n"' ./build/SC\ Echo\ Interrupt.prg | awk '{print toupper($0)}' > ./build/SC\ Echo\ Interrupt.woz
