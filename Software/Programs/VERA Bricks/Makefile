make:
	ca65 -v -l ./build/VERA\ Bricks.list -o ./build/VERA\ Bricks.o ./src/VERA\ Bricks.asm
	cl65 -v -C ../../6502.cfg -o ./build/VERA\ Bricks ./build/VERA\ Bricks.o

view:
	hexdump -C ./build/VERA\ Bricks.prg

woz:
	bin2woz -a 0x0800 ./build/VERA\ Bricks.prg > ./build/VERA\ Bricks.woz
