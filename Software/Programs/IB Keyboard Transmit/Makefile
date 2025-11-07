make:
	ca65 -v -l ./build/IB\ Keyboard\ Transmit.list -o ./build/IB\ Keyboard\ Transmit.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/IB\ Keyboard\ Transmit.prg ./build/IB\ Keyboard\ Transmit.o

view:
	hexdump -C ./build/IB\ Keyboard\ Transmit.prg

woz:
	hexdump -e '"08%02_ax: " 16/1 "%02X " "\n"' ./build/IB\ Keyboard\ Transmit.prg | awk '{print toupper($0)}' > ./build/IB\ Keyboard\ Transmit.woz
