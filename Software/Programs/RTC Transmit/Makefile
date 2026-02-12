make:
	ca65 -v -l ./build/RTC\ Transmit.list -o ./build/RTC\ Transmit.o ./src/RTC\ Transmit.asm
	cl65 -v -C ../../6502.cfg -o ./build/RTC\ Transmit ./build/RTC\ Transmit.o
	
view:
	hexdump -C ./build/RTC\ Transmit.prg

woz:
	bin2woz -a 0x0800 ./build/RTC\ Transmit.prg > ./build/RTC\ Transmit.woz
