make:
	ca65 -v -l ./build/DB\ Mouse\ Info.list -o ./build/DB\ Mouse\ Info.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/DB\ Mouse\ Info.prg ./build/DB\ Mouse\ Info.o

view:
	hexdump -C ./build/DB\ Mouse\ Info.prg

woz:
	hexdump -e '"08%02_ax: " 16/1 "%02X " "\n"' ./build/DB\ Mouse\ Info.prg | awk '{print toupper($0)}' > ./build//DB\ Mouse\ Info.woz
