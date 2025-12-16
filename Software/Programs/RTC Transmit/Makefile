make:
	ca65 -v -l ./build/RTC\ Transmit.list -o ./build/RTC\ Transmit.o ./src/*.asm
	cl65 -v -C ../../6502.cfg -o ./build/RTC\ Transmit ./build/RTC\ Transmit.o
	rm ./build/RTC\ Transmit.bin
	
view:
	hexdump -C ./build/RTC\ Transmit.prg

woz:
	bin2woz -a 0x0800 ./build/RTC\ Transmit.prg > ./build/RTC\ Transmit.woz
