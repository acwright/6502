make:
	ca65 -v -l ./build/DB\ PRAM\ Fill.list -o ./build/DB\ PRAM\ Fill.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/DB\ PRAM\ Fill.prg ./build/DB\ PRAM\ Fill.o

view:
	hexdump -C ./build/DB\ PRAM\ Fill.prg

woz:
	hexdump -e '"08%02_ax: " 16/1 "%02X " "\n"' ./build/DB\ PRAM\ Fill.prg | awk '{print toupper($0)}' > ./build//DB\ PRAM\ Fill.woz
