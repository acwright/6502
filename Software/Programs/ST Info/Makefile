make:
	ca65 -v -l ./build/ST\ Info.list -o ./build/ST\ Info.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/ST\ Info.prg ./build/ST\ Info.o

view:
	hexdump -C ./build/ST\ Info.prg

woz:
	hexdump -e '"08%02_ax: " 16/1 "%02X " "\n"' ./build/ST\ Info.prg | awk '{print toupper($0)}' > ./build/ST\ Info.woz
