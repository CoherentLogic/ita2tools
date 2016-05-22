/*
 * ita2.c
 * 
 *  ITA2 Tools
 *   Converts ASCII to ITA2 and vice-versa
 *
 *  Copyright (C) 2016 Coherent Logic Development LLC
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define SHIFT_FIGS 0x1B
#define SHIFT_LTRS 0x1F

#define ITA2_BOTH 0xFC
#define ITA2_FIGS 0xFD
#define ITA2_LTRS 0xFE
#define ITA2_NONE 0xFF

#define CONV_ASCTOITA 0
#define CONV_ITATOASC 1

/* BOTH SETS = 0xFC; FIGS = 0xFD; LTRS = 0xFE; NO EQUIVALENT = 0xFF */
static const uint8_t ita2_ascii_ltrs[32] = {
  0x00, // NUL
  0x45, // E
  0x0A, // LF
  0x41, // A
  0x20, // SPACE
  0x53, // S
  0x49, // I
  0x55, // U
  0x0D, // CR
  0x44, // D
  0x52, // R
  0x4A, // J
  0x4E, // N
  0x46, // F
  0x43, // C
  0x4B, // K
  0x54, // T
  0x5A, // Z
  0x4C, // L
  0x57, // W
  0x48, // H
  0x59, // Y
  0x50, // P
  0x51, // Q
  0x4F, // O
  0x42, // B
  0x47, // G
  0xFD, // FIGS
  0x4D, // M
  0x58, // X
  0x56, // V
  0xFE  // LTRS
};

static const uint8_t ita2_ascii_figs[32] = {
  0x00, // NUL
  0x33, // 3
  0x0A, // LF
  0x2D, // -
  0x20, // SPACE
  0x27, // '
  0x38, // 8
  0x37, // 7
  0x0D, // CR
  0x05, // ENQ
  0x34, // 4
  0x07, // BEL
  0x2C, // ,
  0x21, // !
  0x3A, // :
  0x28, // (
  0x35, // 5
  0x2B, // +
  0x29, // )
  0x32, // 2
  0x24, // $
  0x36, // 6
  0x30, // 0
  0x31, // 1
  0x39, // 9
  0x3F, // ?
  0x26, // &
  0xFD, // FIGS
  0x2E, // .
  0x2F, // /
  0x3B, // ;
  0xFE  // LTRS
};

/*
 * ASCII -> ITA2 lookup table 
 *
 * SHIFT state is in MSB; encoding in LSB 
 *
 * MSB states:
 *  FC = LSB represents the same character in both LTRS and FIGS
 *  FD = LSB represents a character in FIGS
 *  FE = LSB represents a character in LTRS
 *  FF = LSB represents a character that should be ignored
 *           (no equivalent representation in ITA2 encoding)
 * 
 *  Note that all lowercase characters (ASCII 0x61-0x7A) will
 *  be converted to uppercase (ASCII 0x41-0x5A).
 *  
 *  Double quotes (") will be converted into single quotes (').
 */
static const uint16_t ascii_ita2[128] = {
  0xFC00, // NUL
  0xFFFF, // SOH
  0xFFFF, // STX
  0xFFFF, // ETX
  0xFFFF, // EOT
  0xFD09, // ENQ
  0xFFFF, // ACK
  0xFD0B, // BEL
  0xFFFF, // BS
  0xFFFF, // TAB
  0xFC02, // LF
  0xFFFF, // VT
  0xFFFF, // FF
  0xFC08, // CR
  0xFFFF, // SO
  0xFFFF, // SI
  0xFFFF, // DLE
  0xFFFF, // DC1, XON
  0xFFFF, // DC2
  0xFFFF, // DC3, XOFF
  0xFFFF, // DC4
  0xFFFF, // NAK
  0xFFFF, // SYN
  0xFFFF, // ETB
  0xFFFF, // CAN
  0xFFFF, // EM
  0xFFFF, // SUB
  0xFFFF, // ESC
  0xFFFF, // FS
  0xFFFF, // GS
  0xFFFF, // RS
  0xFFFF, // US
  0xFC04, // SPACE
  0xFD0D, // !
  0xFD27, // " (substitute with single quote here)
  0xFFFF, // #
  0xFD14, // $
  0xFFFF, // %
  0xFD26, // &
  0xFD27, // '
  0xFD0F, // (
  0xFD12, // )
  0xFFFF, // *
  0xFD11, // +
  0xFD0C, // ,
  0xFD03, // -
  0xFD1C, // .
  0xFD1D, // /
  0xFD16, // 0
  0xFD17, // 1
  0xFD13, // 2
  0xFD01, // 3
  0xFD0A, // 4
  0xFD10, // 5
  0xFD15, // 6
  0xFD07, // 7
  0xFD06, // 8
  0xFD18, // 9
  0xFD0E, // :
  0xFD1E, // ;
  0xFFFF, // <
  0xFFFF, // =
  0xFFFF, // >
  0xFD19, // ?
  0xFFFF, // @
  0xFE03, // A
  0xFE19, // B
  0xFE0E, // C
  0xFE09, // D
  0xFE01, // E
  0xFE0D, // F
  0xFE1A, // G
  0xFE14, // H
  0xFE06, // I
  0xFE0B, // J
  0xFE0F, // K
  0xFE12, // L
  0xFE1C, // M
  0xFE0C, // N
  0xFE18, // O
  0xFE16, // P
  0xFE17, // Q
  0xFE0A, // R
  0xFE05, // S
  0xFE10, // T
  0xFE07, // U
  0xFE1E, // V
  0xFE13, // W
  0xFE1D, // X
  0xFE15, // Y
  0xFE11, // Z
  0xFFFF, // [
  0xFFFF, /* \ */
  0xFFFF, // ]
  0xFFFF, // ^
  0xFFFF, // _
  0xFFFF, // `
  0xFE03, // A
  0xFE19, // B
  0xFE0E, // C
  0xFE09, // D
  0xFE01, // E
  0xFE0D, // F
  0xFE1A, // G
  0xFE14, // H
  0xFE06, // I
  0xFE0B, // J
  0xFE0F, // K
  0xFE12, // L
  0xFE1C, // M
  0xFE0C, // N
  0xFE18, // O
  0xFE16, // P
  0xFE17, // Q
  0xFE0A, // R
  0xFE05, // S
  0xFE10, // T
  0xFE07, // U
  0xFE1E, // V
  0xFE13, // W
  0xFE1D, // X
  0xFE15, // Y
  0xFE11, // Z
  0xFFFF, // {
  0xFFFF, // |
  0xFFFF, // }
  0xFFFF, // ~
  0xFFFF  // DEL
};

void ita2asc(char);
void asc2ita(char);

uint8_t shift_state;

int main(int argc, char **argv) 
{
  void (*conv)(char);
  char c;

  shift_state = SHIFT_LTRS;

  if(argc != 2) {
    printf("usage:  ita2 [a|i]\n");
    printf(" a = convert ASCII to ITA2\n");
    printf(" i = convert ITA2 to ASCII\n");
    return 1;
  }
  
  switch(argv[1][0]) {
  case 'a':
    /* it is customary to begin an ITA2 output with two LTRS characters */
    putchar(SHIFT_LTRS);
    putchar(SHIFT_LTRS);

    conv = &asc2ita;
    break;
  case 'i':
    conv = &ita2asc;
    break;
  }

  while((c = getchar()) != EOF) {
    (*conv)(c);
  }
 
}

void ita2asc(char c) 
{
  uint8_t conv;

  switch(c) {
  case SHIFT_FIGS:
    shift_state = SHIFT_FIGS;
    break;
  case SHIFT_LTRS:
    shift_state = SHIFT_LTRS;
    break;
  default:
    if(shift_state == SHIFT_FIGS) {
      conv = ita2_ascii_figs[c];
    }
    else {
      conv = ita2_ascii_ltrs[c];
    }
    
    putchar((char) conv);
  }
}


void asc2ita(char c)
{
  uint16_t conv;
  uint8_t lsb;
  uint8_t msb;

    
  c &= 0x7F; // mask off most significant bit (we want pure ASCII)

  conv = ascii_ita2[c];
  
  lsb = (uint8_t) (conv & 0x00FF);
  msb = (uint8_t) (conv >> 8) & 0xff;

  switch(msb) {
  case ITA2_BOTH: // no change in shift state for this char
    
    putchar((char) lsb);      
    break;

  case ITA2_FIGS: // following char = FIGS

    if(shift_state != SHIFT_FIGS) {
      putchar(SHIFT_FIGS);
      shift_state = SHIFT_FIGS;
    }

    putchar((char) lsb);
 
    break;

  case ITA2_LTRS: // following char = LTRS
      
    if(shift_state != SHIFT_LTRS) {
      putchar(SHIFT_LTRS);
      shift_state = SHIFT_LTRS;
    }      

    putchar((char) lsb);
      
    break;

  case ITA2_NONE: // this char cannot be represented in ITA2
    break;
  }    

}
