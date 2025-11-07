make:
	ca65 -v -l ./build/GPIO\ Input.list -o ./build/GPIO\ Input.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/GPIO\ Input.prg ./build/GPIO\ Input.o

view:
	hexdump -C ./build/GPIO\ Input.prg

woz:
	hexdump -e '"08%02_ax: " 16/1 "%02X " "\n"' ./build/GPIO\ Input.prg | awk '{print toupper($0)}' > ./build/GPIO\ Input.woz
