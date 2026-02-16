TARGET = NOP
CONFIG = NOP

.PHONY: all clean

all: build

build: $(TARGET).asm
	cl65 -C $(CONFIG).cfg -l $(TARGET).lst -o $(TARGET).bin $(TARGET).asm 
	
view:
	hexdump -C $(TARGET).bin

eeprom:
	minipro -p AT28C256	-w NOP.bin

clean:
	rm $(TARGET).bin
	rm $(TARGET).lst