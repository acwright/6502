make:
	ca65 -v -l ./build/DB\ Joystick\ Info.list -o ./build/DB\ Joystick\ Info.o ./src/DB\ Joystick\ Info.asm
	cl65 -v -C ../../6502.cfg -o ./build/DB\ Joystick\ Info ./build/DB\ Joystick\ Info.o
	
view:
	hexdump -C ./build/DB\ Joystick\ Info.prg

woz:
	bin2woz -a 0x0800 ./build/DB\ Joystick\ Info.prg > ./build//DB\ Joystick\ Info.woz
