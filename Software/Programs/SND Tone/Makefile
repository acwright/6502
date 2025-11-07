make:
	ca65 -v -l ./build/SND\ Tone.list -o ./build/SND\ Tone.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/SND\ Tone.prg ./build/SND\ Tone.o

view:
	hexdump -C ./build/SND\ Tone.prg

woz:
	hexdump -e '"08%02_ax: " 16/1 "%02X " "\n"' ./build/SND\ Tone.prg | awk '{print toupper($0)}' > ./build/SND\ Tone.woz
