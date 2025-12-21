make:
	ca65 -v -l ./build/OB\ Bricks.list -o ./build/OB\ Bricks.o ./src/*.asm
	cl65 -v -C ../../6502.cfg -o ./build/OB\ Bricks ./build/OB\ Bricks.o
	rm ./build/OB\ Bricks.bin

view:
	hexdump -C ./build/OB\ Bricks.prg

woz:
	bin2woz -a 0x0800 ./build/OB\ Bricks.prg > ./build/OB\ Bricks.woz
