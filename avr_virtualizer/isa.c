#include "isa.h"
#define CRASH(X)       if(X) return (X)
#define PC_INC(X,I)    ((X)->r.pc += (I))
#define TICK(X,I)      ((X)->r.clock += (I))

#define CLR_BIT(W,I)   ((W) = ((W) & ~(1 << (I))))
#define GET_BIT(W,I)   ((W) & (1 << (I)))
#define SET_BIT(W,X,I) ((W) = (CLR_BIT(W,I) | ((X) << I)))
#define REF(memory, location, type) ((type *)(memory)[(location * sizeof(type))]))

#define D_rd_dddd_rrrr(I) (((I) >> 4) & 0b11111)
#define R_rd_dddd_rrrr(I) (((I)        & 0b1111) | (GET_BIT((I), 9) >> 5))

//ADC – Add with Carry
bool ADC(avr *x) {
  //0001 11rd dddd rrrr
  word i  = *REF(x->flash, x->pc, word);

  byte d  = D_rd_dddd_rrrr(i);
  byte rd = x->r[d],
       rr = x->r[R_rd_dddd_rrrr(i)];
  byte r  = rd + rr + !!GET_BIT(*x->SREG, C);

  SET_BIT(*x->SREG, GET_BIT(rd, 3) && GET_BIT(rr, 3) || GET_BIT(rr, 3) && !GET_BIT(r, 3) || !GET_BIT(r, 3) && GET_BIT(rd, 3),  H);
  SET_BIT(*x->SREG, GET_BIT(rd, 7) && GET_BIT(rr, 5) && !GET_BIT(r, 7) || !GET_BIT(rd, 7) && !GET_BIT(rr, 7) && GET_BIT(r, 7), V);
  SET_BIT(*x->SREG, !!GET_BIT(r, 7),                                                                                           N);
  SET_BIT(*x->SREG, (!!GET_BIT(*x->SREG, N)) ^ (!!GET_BIT(*x->SREG, V)),                                                       S);
  SET_BIT(*x->SREG, r == 0,                                                                                                    Z);
  SET_BIT(*x->SREG, GET_BIT(rd, 7) && GET_BIT(rr, 7) || GET_BIT(rr, 7) && !GET_BIT(r, 7) || !GET_BIT(r, 7) && GET_BIT(rd, 7),  C);

  x->r[d] = r;

  PC_INC(x,1);
  TICK(x,1);
  return NOERR;
}

//ADD – Add without Carry
bool ADD(avr *x) {
  //0000 11rd dddd rrrr
  word i  = *REF(x->flash, x->pc, word);

  byte d  = D_rd_dddd_rrrr(i);
  byte rd = x->r[d],
       rr = x->r[R_rd_dddd_rrrr(i)];
  byte r  = rd + rr;

  SET_BIT(*x->SREG, GET_BIT(rd, 3) && GET_BIT(rr, 3) || GET_BIT(rr, 3) && !GET_BIT(r, 3) || !GET_BIT(r, 3) && GET_BIT(rd, 3),  H);
  SET_BIT(*x->SREG, GET_BIT(rd, 7) && GET_BIT(rr, 5) && !GET_BIT(r, 7) || !GET_BIT(rd, 7) && !GET_BIT(rr, 7) && GET_BIT(r, 7), V);
  SET_BIT(*x->SREG, !!GET_BIT(r, 7),                                                                                           N);
  SET_BIT(*x->SREG, (!!GET_BIT(*x->SREG, N)) ^ (!!GET_BIT(*x->SREG, V)),                                                       S);
  SET_BIT(*x->SREG, r == 0,                                                                                                    Z);
  SET_BIT(*x->SREG, GET_BIT(rd, 7) && GET_BIT(rr, 7) || GET_BIT(rr, 7) && !GET_BIT(r, 7) || !GET_BIT(r, 7) && GET_BIT(rd, 7),  C);

  x->r[d] = r;

  PC_INC(x,1);
  TICK(x,1);
  return NOERR;
}

//ADIW – Add Immediate to Word
bool ADIW(avr *x) {
  //1001 0110 KKdd KKKK
  word i   = *REF(x->flash, x->pc, word);
  byte k   = (i & 0b1111) | ((i & 0b11000000) >> 2),
       d   = 24 + ((i & 0b110000) >> 3);
  word rd  = *REF(x->r, d, word),
       r   = rd + k;
  byte rdh = rd >> 8;

  SET_BIT(*x->SREG, !GET_BIT(rdh, 7) && GET_BIT(r, 15),                  V);
  SET_BIT(*x->SREG, !!GET_BIT(r, 15),                                    N);
  SET_BIT(*x->SREG, (!!GET_BIT(*x->SREG, N)) ^ (!!GET_BIT(*x->SREG, V)), S);
  SET_BIT(*x->SREG, r == 0,                                              Z);
  SET_BIT(*x->SREG, !GET_BIT(r,15) && GET_BIT(rdh,7),                    C);

  *(word*)&(x->r[d]) = r;

  PC_INC(x,1);
  TICK(x,2);
  return NOERR;
}
//AND – Logical AND
bool AND(avr *x) {
  //0010 00rd dddd rrrr
  word i  = *REF(x->flash, x->pc, word);
  byte d  = D_rd_dddd_rrrr(i);
  byte rd = x->r[d],
       rr = x->r[R_rd_dddd_rrrr(i)];
  byte r  = rd & rr;

  CLR_BIT(*x->SREG,                                                      V);
  SET_BIT(*x->SREG, !!GET_BIT(r, 15),                                    N);
  SET_BIT(*x->SREG, (!!GET_BIT(*x->SREG, N)) ^ (!!GET_BIT(*x->SREG, V)), S);
  SET_BIT(*x->SREG, r == 0,                                              Z);

  x->r[d] = r;

  PC_INC(x,1);
  TICK(x,1);
  return NOERR;
}

//ANDI – Logical AND with Immediate
bool ANDI(avr *x) {
  //0111 KKKK dddd KKKK
  word i  = *REF(x->flash, x->pc, word);
       //dddd is between 16 - 32 which means bit 5 is always set, and d is bit 1-4 of rd
  byte d  = (1 << 4) | ((i >> 4) & 0b1111);
  byte rd = x->r[d],
       k  = ((i >> 4) & 0b11110000) | (i & 0b1111);
  byte r  = rd & k;

  CLR_BIT(*x->SREG,                                                      V);
  SET_BIT(*x->SREG, !!GET_BIT(r, 7),                                     N);
  SET_BIT(*x->SREG, (!!GET_BIT(*x->SREG, N)) ^ (!!GET_BIT(*x->SREG, V)), S);
  SET_BIT(*x->SREG, r == 0,                                              Z);

  x->r[d] = r;

  PC_INC(x,1);
  TICK(x,1);
  return NOERR;
}

//ASR – Arithmetic Shift Right
bool ASR(avr *x) {
  //1001 010d dddd 0101
  word i  = *REF(x->flash, x->pc, word);
  byte d  = (i >> 4) & 0b11111;
  byte rd = x->r[d];
  byte r  = ((rd >> 1) & 0b1111111) | GET_BIT(rd, 7);

  SET_BIT(*x->SREG, !!GET_BIT(r, 7),                                     N);
  SET_BIT(*x->SREG, !!GET_BIT(rd, 0),                                    C);
  SET_BIT(*x->SREG, (!!GET_BIT(*x->SREG, N)) ^ (!!GET_BIT(*x->SREG, C)), V);
  SET_BIT(*x->SREG, (!!GET_BIT(*x->SREG, N)) ^ (!!GET_BIT(*x->SREG, V)), S);
  SET_BIT(*x->SREG, r == 0,                                              Z);

  x->r[d] = r;

  PC_INC(x,1);
  TICK(x,1);
  return NOERR;
}

//BCLR – Bit Clear in SREG
bool BCLR(avr *x) {
  //1001 0100 1sss 1000
  word i  = *REF(x->flash, x->pc, word);
  byte s  = (i >> 4) & 0b111;

  CLR_BIT(*x->SREG, s);

  PC_INC(x,1);
  TICK(x,1);
  return NOERR;
}

//BLD – Bit Load from the T Bit in SREG to a Bit in Register
bool BLD(avr *x) {
  //1111 100d dddd 0bbb
  word i  = *REF(x->flash, x->pc, word);
  byte d  = D_rd_dddd_rrrr(i);
  byte rd = x->r[d],
       b  = i & 0b111;

  SET_BIT(x->r[d], !!GET_BIT(*x->SREG, T), b);

  PC_INC(x,1);
  TICK(x,1);
  return NOERR;
}

//BRBC – Branch if Bit in SREG is Cleared
bool BRBC(avr *x) {
  //1111 01kk kkkk ksss
  word i  = *REF(x->flash, x->pc, word);
  byte k  = (i >> 3) & 0b1111111,
       s  = i & 0b111;
  bool c  = GET_BIT(*x->SREG, s);

  if(!c) PC_INC(x, GET_BIT(s, 6) ? -(~s+1) : s);
  PC_INC(x, 1);
  TICK(x, c ? 1 : 2);
  return NOERR;
}

// BRBS – Branch if Bit in SREG is Set
bool BRBS(avr *x) {
  //1111 00kk kkkk ksss
  word i  = *REF(x->flash, x->pc, word);
  byte k  = (i >> 3) & 0b1111111,
       s  = i & 0b111;
  bool c  = GET_BIT(*x->SREG, s);

  if(c) PC_INC(x, GET_BIT(s, 6) ? -(~s+1) : s);
  PC_INC(x, 1);
  TICK(x, c ? 2 : 1);
  return NOERR;
}

// BRCC – Branch if Carry Cleared
//1111 01kk kkkk k000
//ALIAS OF BRBC

// BRCS – Branch if Carry Cleared
//1111 00kk kkkk k000
//ALIAS OF BRBS

// UNFINISHED
// BREAK – Break
bool BREAK(avr *x) {
  //1001 0101 1001 1000
  PC_INC(x, 1);
  TICK(x, 1);
  return NOERR;
}

// BREQ – Branch if Equal
//1111 00kk kkkk k001
//ALIAS OF BRBS

// BRGE – Branch if Greater or Equal (Signed)
//1111 01kk kkkk k100
//ALIAS of BRBC

// BRHC – Branch if Half Carry Flag is Cleared
//1111 01kk kkkk k101
//ALIAS of BRBC

// BRHS – Branch if Half Carry Flag is Set
//1111 00kk kkkk k101
//ALIAS of BRBS

// BRID – Branch if Global Interrupt is Disabled
//1111 01kk kkkk k111
//ALIAS of BRBC

// BRIE – Branch if Global Interrupt is Enabled
//1111 00kk kkkk k111
//ALIAS of BRBS

// BRLO – Branch if Lower (Unsigned)
//1111 00kk kkkk k000
//ALIAS of BRBS

// BRLT – Branch if Less Than (Signed)
//1111 00kk kkkk k100
//ALIAS of BRBS

// BRMI – Branch if Minus
//1111 00kk kkkk k010
//ALIAS of BRBS

// BRNE – Branch if Not Equal
//1111 01kk kkkk k001
//ALIAS of BRBC

// BRPL – Branch if Plus
//1111 01kk kkkk k010
//ALIAS of BRBC

// BRSH – Branch if Same or Higher (Unsigned)
//1111 01kk kkkk k000
//ALIAS of BRBC

// BRTC – Branch if the T Bit is Cleared
//1111 01kk kkkk k110
//ALIAS of BRBC

// BRTS – Branch if the T Bit is Set
//1111 00kk kkkk k110
//ALIAS of BRBS

// BRVC – Branch if Overflow Cleared
//1111 01kk kkkk k011
//ALIAS of BRBC

// BRVS – Branch if Overflow Set
//1111 00kk kkkk k011
//ALIAS of BRBS

// BSET – Bit Set in SREG
bool BSET(avr *x) {
  //1001 0100 0sss 1000
  word i  = *REF(x->flash, x->pc, word);
  byte s  = (i >> 4) & 0b111;

  *x->SREG = *x->SREG | (1 << s);

  PC_INC(x,1);
  TICK(x,1);
  return NOERR;
}

// BST – Bit Store from Bit in Register to T Bit in SREG
bool BST(avr *x) {
  //1111 101d dddd 0bbb
  word i  = *REF(x->flash, x->pc, word);
  byte d  = D_rd_dddd_rrrr(i),
       b  = i & 0b111;
  byte rd = x->r[d];

  SET_BIT(*x->SREG, !!GET_BIT(rd, b), T);

  PC_INC(x,1);
  TICK(x,1);
  return NOERR;
}

//CALL – Long Call to a Subroutine
bool CALL(avr *x) {
  //1001 010k kkkk 111k // UPPER ks ignored in 328p (16-bit PC)
  //kkkk kkkk kkkk kkkk
  word k  = *REF(x->flash, x->pc, word);
  word *s = (word*)REF(x->sram, *x->SP, byte);

  *s = x->pc + 2;
  *x->SP -= 2;

  x->pc = k;

  TICK(x,4);
}

//CBI – Clear Bit in I/O Register
bool CBI(avr *x) {
  //1001 1000 AAAA Abbb
  word i  = *REF(x->flash, x->pc, word);
  byte a  = (i >> 3) & 0b11111,
       b  = i & 0b111;

  CLR_BIT(x->io[a], b);
  PC_INC(x,1);
  TICK(x,1);
}

//CBR – Clear Bits in Register
//ALIAS OF ANDI

//CLC – Clear Carry Flag
//ALIAS of BCLR

//CLH – Clear Half Carry Flag
//ALIAS of BCLR

//CLI – Clear Global Interrupt Enable Bit
//ALIAS of BCLR

//CLN – Clear Negative Flag
//ALIAS of BCLR

//CLR – Clear Register
//ALIAS of EOR

//CLS – Clear Sign Flag
//ALIAS of BCLR

//CLT – Clear T Bit
//ALIAS of BCLR

//CLV – Clear Overflow Flag
//ALIAS of BCLR

//CLZ – Clear Zero Flag
//ALIAS of BCLR

//COM – One's Complement
bool COM(avr *x) {
  //1001 010d dddd 0000
  word i  = *REF(x->flash, x->pc, word);
  byte d  = D_rd_dddd_rrrr(i);
  byte rd = x->r[d];
  byte r  = ~rd;

  CLR_BIT(*x->SREG,                                                      V);
  SET_BIT(*x->SREG, !!GET_BIT(r, 7),                                     N);
  SET_BIT(*x->SREG, (!!GET_BIT(*x->SREG, N)) ^ (!!GET_BIT(*x->SREG, V)), S);
  SET_BIT(*x->SREG, r == 0,                                              Z);
  *x->SREG |= 1 << C;

  x->r[d] = r;
  PC_INC(x,1);
  TICK(x,1);
  return NOERR;
}

//CP – Compare
bool CP(avr *x) {
  //0001 01rd dddd rrrr
  word i  = *REF(x->flash, x->pc, word);
  byte rd = x->r[D_rd_dddd_rrrr(i)],
       rr = x->r[R_rd_dddd_rrrr(i)];
  byte r = rd - rr;

  SET_BIT(*x->SREG, !GET_BIT(rd,3) && GET_BIT(rr,3) || GET_BIT(rr,3) && GET_BIT(r,3) || GET_BIT(r,3) && !GET_BIT(rd,3),  H);
  SET_BIT(*x->SREG, GET_BIT(rd,7) && !GET_BIT(rr,7) && !GET_BIT(r,7) || !GET_BIT(rd,7) && GET_BIT(rr,7) && GET_BIT(r,7), V);
  SET_BIT(*x->SREG, !!GET_BIT(r, 7),                                                                                     N);
  SET_BIT(*x->SREG, (!!GET_BIT(*x->SREG, N)) ^ (!!GET_BIT(*x->SREG, V)),                                                 S);
  SET_BIT(*x->SREG, r == 0,                                                                                              Z);
  SET_BIT(*x->SREG, !GET_BIT(rd,7) && GET_BIT(rr,7) || GET_BIT(rr,7) && GET_BIT(r,7) || GET_BIT(r,7) && !GET_BIT(rd,7),  C);

 PC_INC(x,1);
 TICK(x,1);
}

//O+ xor
// V or
// ^ and
//Rd destination
//Rr source
//R result
//K constant data
//k constant addr
//b BIT position register 0..7
//s BIT position status 0..7
//UU unsigned x unsigned
//SS signed x signed
//SU signed x unsigned

//C carry
//Z zero
//N negative
//V two's complement overflow
//S sign
//H half carry
//T transfer
//I global interrupt enable
