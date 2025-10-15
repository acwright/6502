make:
	ca65 -v -l ./build/RTC\ Transmit.list -o ./build/RTC\ Transmit.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/RTC\ Transmit.bin ./build/RTC\ Transmit.o

view:
	hexdump -C ./build/RTC\ Transmit.bin

eeprom:
	minipro -p AT28C256	-w ./build/RTC\ Transmit.bin