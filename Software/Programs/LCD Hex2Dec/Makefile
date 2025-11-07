make:
	ca65 -v -l ./build/LCD\ Hex2Dec.list -o ./build/LCD\ Hex2Dec.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/LCD\ Hex2Dec.prg ./build/LCD\ Hex2Dec.o

view:
	hexdump -C ./build/LCD\ Hex2Dec.prg

woz:
	hexdump -e '"08%02_ax: " 16/1 "%02X " "\n"' ./build/LCD\ Hex2Dec.prg | awk '{print toupper($0)}' > ./build/LCD\ Hex2Dec.woz
