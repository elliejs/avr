#ifndef AVRLIB
#define AVRLIB

// #include <stddef.h>
#include <stdint.h>

#define PAGE_SIZE   256
#define EEPROM_SIZE 1024
#define SRAM_SIZE   2048
#define FLASH_SIZE  4096

typedef uint8_t  byte;
typedef uint16_t word;
#define BYTE        8
#define WORD        16

typedef char     bool;
#define FALSE 0
#define NOERR 0
#define TRUE 1


#define BIT(X) (1 << (X))

enum SREG {
  C = 0,
  Z = 1,
  N = 2,
  V = 3,
  S = 4,
  H = 5,
  T = 6,
  I = 7
};

enum ADCSRA {
  ADPS0 = BIT(0),
  ADPS1 = BIT(1),
  ADPS2 = BIT(2),
  ADIE  = BIT(3),
  ADIF  = BIT(4),
  ADATE = BIT(5),
  ADCS  = BIT(6),
  ADEN  = BIT(7)
};
enum ADCSRB {
  ADTS0 = BIT(0),
  ADTS1 = BIT(1),
  ADTS2 = BIT(2),
  ACME  = BIT(6)
};
enum ADMUX {
  MUX0  = BIT(0),
  MUX1  = BIT(1),
  MUX2  = BIT(2),
  MUX3  = BIT(3),
  ADLAR = BIT(5),
  REFS0 = BIT(6),
  REFS1 = BIT(7)
};
enum PINB {
  PINB0 = BIT(0),
  PINB1 = BIT(1),
  PINB2 = BIT(2),
  PINB3 = BIT(3),
  PINB4 = BIT(4),
  PINB5 = BIT(5),
  PINB6 = BIT(6),
  PINB7 = BIT(7)
};
enum DDRB {
  DDRB0 = BIT(0),
  DDRB1 = BIT(1),
  DDRB2 = BIT(2),
  DDRB3 = BIT(3),
  DDRB4 = BIT(4),
  DDRB5 = BIT(5),
  DDRB6 = BIT(6),
  DDRB7 = BIT(7)
};
enum PORTB {
  PORTB0 = BIT(0),
  PORTB1 = BIT(1),
  PORTB2 = BIT(2),
  PORTB3 = BIT(3),
  PORTB4 = BIT(4),
  PORTB5 = BIT(5),
  PORTB6 = BIT(6),
  PORTB7 = BIT(7)
};
enum PINC {
  PINC0 = BIT(0),
  PINC1 = BIT(1),
  PINC2 = BIT(2),
  PINC3 = BIT(3),
  PINC4 = BIT(4),
  PINC5 = BIT(5),
  PINC6 = BIT(6)
};
enum DDRC {
  DDRC0 = BIT(0),
  DDRC1 = BIT(1),
  DDRC2 = BIT(2),
  DDRC3 = BIT(3),
  DDRC4 = BIT(4),
  DDRC5 = BIT(5),
  DDRC6 = BIT(6)
};
enum PORTC {
  PORTC0 = BIT(0),
  PORTC1 = BIT(1),
  PORTC2 = BIT(2),
  PORTC3 = BIT(3),
  PORTC4 = BIT(4),
  PORTC5 = BIT(5),
  PORTC6 = BIT(6)
};
enum PIND {
  PIND0 = BIT(0),
  PIND1 = BIT(1),
  PIND2 = BIT(2),
  PIND3 = BIT(3),
  PIND4 = BIT(4),
  PIND5 = BIT(5),
  PIND6 = BIT(6),
  PIND7 = BIT(7)
};
enum DDRD {
  DDRD0 = BIT(0),
  DDRD1 = BIT(1),
  DDRD2 = BIT(2),
  DDRD3 = BIT(3),
  DDRD4 = BIT(4),
  DDRD5 = BIT(5),
  DDRD6 = BIT(6),
  DDRD7 = BIT(7)
};
enum PORTD {
  PORTD0 = BIT(0),
  PORTD1 = BIT(1),
  PORTD2 = BIT(2),
  PORTD3 = BIT(3),
  PORTD4 = BIT(4),
  PORTD5 = BIT(5),
  PORTD6 = BIT(6),
  PORTD7 = BIT(7)
};
enum MCUCR {
  IVCE = BIT(0),
  IVSEL = BIT(1),
  PUD = BIT(4),
  BODSE = BIT(5),
  BODS = BIT(6)
};
enum PRR {
  PRADC = BIT(0),
  PRUSART0 = BIT(1),
  PRSPI0 = BIT(2),
  PRTIM1 = BIT(3),
  PRTIM0 = BIT(5),
  PRTIM2 = BIT(6),
  PRTWI0 = BIT(7)
};
enum TIFR0 {
  TOV = BIT(1),
  OCFA = BIT(2),
  OFCB = BIT(3)
};
enum GTCCR {
  PSRSYNC = BIT(0),
  PSRASY = BIT(1),
  TSM = BIT(7)
};
enum TCCR0A {
  WGM00 = BIT(0),
  WGM01 = BIT(1),
  COM0B0 = BIT(4),
  COM0B1 = BIT(5),
  COM0A0 = BIT(6),
  COM0A1 = BIT(7)
};
enum TCCR0B {
  CS0 = BIT(0),
  WGM02 = BIT(3),
  FOC0B = BIT(6),
  FOC0A = BIT(7)
};
enum TIMSK0 {
  TOIE = BIT(0),
  OCIEA = BIT(1),
  OCIEB = BIT(2)
};

typedef struct {
  unsigned long long clock;
  byte *eeprom, *flash, *sram;
  byte *r, *io, *eio;
  word pc;
  byte *SREG;
  word *SP;
  // enum ADCSRA *ADCSRA;
  // enum ADCSRB *ADCSRB;
  // enum ADMUX *ADMUX;
  // enum PINB *PINB;
  // enum DDRB *DDRB;
  // enum PORTB *PORTB;
  // enum PINC *PINC;
  // enum DDRC *DDRC;
  // enum PORTC *PORTC;
  // enum PIND *PIND;
  // enum DDRD *DDRD;
  // enum PORTD *PORTD;
  // enum MCUCR *MCUCR;
  // enum PRR *PRR;
  // enum TIFR0 *TIFR0;
  // enum GTCCR *GTCCR;
  // enum TCCR0A *TCCR0A;
  // enum TCCR0B *TCCR0B;
  // enum TIMSK0 *TIMSK0;
  // enum SREG *SREG;
} avr;

#endif
