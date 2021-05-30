#include <stdio.h>
#include <stdlib.h>
#include "isa.h"

avr avr_init() {
  avr new = {
    .clock = 0,

    .eeprom = calloc(EEPROM_SIZE, BYTE),
    .flash  = calloc(FLASH_SIZE,  BYTE),
    .sram   = calloc(SRAM_SIZE ,  BYTE),

    .r      = REF(new.sram, 0,    byte),
    .io     = REF(new.sram, 0x20, byte),
    .eio    = REF(new.sram, 0x60, byte),

    .pc     = 0,

    .SREG   = REF(new.sram, 0x3F, byte),

    .SP     = (word*)REF(new.io, 0x3D, byte)

    // .ADCSRA = REF(),
    // .ADCSRB = REF(),
    // .ADMUX = REF(),
    // .PINB = REF(),
    // .DDRB = REF(),
    // .PORTB = REF(),
    // .PINC = REF(),
    // .DDRC = REF(),
    // .PORTC = REF(),
    // .PIND = REF(),
    // .DDRD = REF(),
    // .PORTD = REF(),
    // .MCUCR = REF(),
    // .PRR = REF(),
    // .TIFR0 = REF(),
    // .GTCCR = REF(),
    // .TCCR0A = REF(),
    // .TCCR0B = REF(),
    // .TIMSK0 = REF()
  };
  return new;
}

int main(int argc, char const *argv[]) {
  avr chip = avr_init();

  return 0;
}
