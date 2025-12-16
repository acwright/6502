make:
	ca65 -v -l ./build/VH\ Bricks.list -o ./build/VH\ Bricks.o ./src/*.asm
	cl65 -v -C ../../6502.cfg -o ./build/VH\ Bricks ./build/VH\ Bricks.o
	rm ./build/VH\ Bricks.bin

view:
	hexdump -C ./build/VH\ Bricks.prg

woz:
	bin2woz -a 0x0800 ./build/VH\ Bricks.prg > ./build/VH\ Bricks.woz
