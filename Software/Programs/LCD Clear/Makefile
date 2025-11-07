make:
	ca65 -v -l ./build/LCD\ Clear.list -o ./build/LCD\ Clear.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/LCD\ Clear.prg ./build/LCD\ Clear.o

view:
	hexdump -C ./build/LCD\ Clear.prg

woz:
	hexdump -e '"08%02_ax: " 16/1 "%02X " "\n"' ./build/LCD\ Clear.prg | awk '{print toupper($0)}' > ./build/LCD\ Clear.woz
