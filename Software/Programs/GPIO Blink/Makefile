make:
	ca65 -v -l ./build/GPIO\ Blink.list -o ./build/GPIO\ Blink.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/GPIO\ Blink.prg ./build/GPIO\ Blink.o

view:
	hexdump -C ./build/GPIO\ Blink.prg

woz:
	hexdump -e '"08%02_ax: " 16/1 "%02X " "\n"' ./build/GPIO\ Blink.prg | awk '{print toupper($0)}' > ./build/GPIO\ Blink.woz
