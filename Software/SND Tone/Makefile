make:
	ca65 -v -l ./build/SND\ Tone.list -o ./build/SND\ Tone.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/SND\ Tone.bin ./build/SND\ Tone.o

view:
	hexdump -C ./build/SND\ Tone.bin

eeprom:
	minipro -p AT28C256	-w ./build/SND\ Tone.bin