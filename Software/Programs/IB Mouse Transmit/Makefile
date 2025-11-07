make:
	ca65 -v -l ./build/IB\ Mouse\ Transmit.list -o ./build/IB\ Mouse\ Transmit.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/IB\ Mouse\ Transmit.prg ./build/IB\ Mouse\ Transmit.o

view:
	hexdump -C ./build/IB\ Mouse\ Transmit.prg

woz:
	hexdump -e '"08%02_ax: " 16/1 "%02X " "\n"' ./build/IB\ Mouse\ Transmit.prg | awk '{print toupper($0)}' > ./build/IB\ Mouse\ Transmit.woz
