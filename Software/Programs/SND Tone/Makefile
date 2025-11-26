make:
	ca65 -v -l ./build/SND\ Tone.list -o ./build/SND\ Tone.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/SND\ Tone.prg ./build/SND\ Tone.o

view:
	hexdump -C ./build/SND\ Tone.prg

woz:
	bin2woz -a 0x0800 ./build/SND\ Tone.prg > ./build/SND\ Tone.woz
