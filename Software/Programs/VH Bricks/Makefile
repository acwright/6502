make:
	ca65 -v -l ./build/VH\ Bricks.list -o ./build/VH\ Bricks.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/VH\ Bricks.prg ./build/VH\ Bricks.o

view:
	hexdump -C ./build/VH\ Bricks.prg

woz:
	hexdump -e '"08%02_ax: " 16/1 "%02X " "\n"' ./build/VH\ Bricks.prg | awk '{print toupper($0)}' > ./build/VH\ Bricks.woz
