make:
	ca65 -v -l ./build/IB\ SNES\ Transmit.list -o ./build/IB\ SNES\ Transmit.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/IB\ SNES\ Transmit.prg ./build/IB\ SNES\ Transmit.o

view:
	hexdump -C ./build/IB\ SNES\ Transmit.prg

woz:
	hexdump -e '"08%02_ax: " 16/1 "%02X " "\n"' ./build/IB\ SNES\ Transmit.prg | awk '{print toupper($0)}' > ./build/IB\ SNES\ Transmit.woz
