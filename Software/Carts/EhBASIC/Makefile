make:
	ca65 -v -l ./build/EhBASIC.list -o ./build/EhBASIC.o ./src/EhBASIC.asm --feature labels_without_colons
	cl65 -v -C EhBASIC.cfg -o ./build/EhBASIC.bin ./build/EhBASIC.o

view:
	hexdump -C ./build/EhBASIC.bin

eeprom:
	minipro -p AT28C256	-w ./build/EhBASIC.bin