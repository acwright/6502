make:
	ca65 -v -l ./build/DB\ Joystick\ Info.list -o ./build/DB\ Joystick\ Info.o ./src/*.asm
	cl65 -v -C 6502.cfg -o ./build/DB\ Joystick\ Info.prg ./build/DB\ Joystick\ Info.o

view:
	hexdump -C ./build/DB\ Joystick\ Info.prg

woz:
	hexdump -e '"08%02_ax: " 16/1 "%02X " "\n"' ./build/DB\ Joystick\ Info.prg | awk '{print toupper($0)}' > ./build//DB\ Joystick\ Info.woz
