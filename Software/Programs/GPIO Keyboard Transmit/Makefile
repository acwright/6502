TARGET = GPIO\ Keyboard\ Transmit
CONFIG = 6502

.PHONY: all clean

all: build woz

build: $(TARGET).asm
	cl65 -C ../../$(CONFIG).cfg -l $(TARGET).lst -o $(TARGET).prg $(TARGET).asm 
	
view:
	hexdump -C $(TARGET).prg

woz:
	bin2woz -a 0x0800 $(TARGET).prg > $(TARGET).woz

clean:
	rm $(TARGET).prg
	rm $(TARGET).woz
	rm $(TARGET).lst
