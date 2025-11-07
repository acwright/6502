make:
	ca65 -v -l ./build/LCD\ Hello\ World.list -o ./build/LCD\ Hello\ World.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/LCD\ Hello\ World.prg ./build/LCD\ Hello\ World.o

view:
	hexdump -C ./build/LCD\ Hello\ World.prg

woz:
	hexdump -e '"08%02_ax: " 16/1 "%02X " "\n"' ./build/LCD\ Hello\ World.prg | awk '{print toupper($0)}' > ./build/LCD\ Hello\ World.woz
