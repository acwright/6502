make:
	ca65 -v -l ./build/RTC\ Transmit.list -o ./build/RTC\ Transmit.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/RTC\ Transmit.prg ./build/RTC\ Transmit.o

view:
	hexdump -C ./build/RTC\ Transmit.prg

woz:
	hexdump -e '"08%02_ax: " 16/1 "%02X " "\n"' ./build/RTC\ Transmit.prg | awk '{print toupper($0)}' > ./build/RTC\ Transmit.woz
