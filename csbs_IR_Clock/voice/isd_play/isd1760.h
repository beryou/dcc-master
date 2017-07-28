#ifndef _ISD1760_H_
#define _ISD1760_H_

// ISD1760ЦёБо
#define PU          0x01
#define STOP        0x02
#define RESTE       0x03
#define CLR_INT     0x04
#define RD_STATUS   0x05
#define RD_PLAY_PTR 0x06
#define PD          0x07
#define RD_REC_PTR  0x08
#define DEVID       0x09

#define PLAY        0x40
#define REC         0x41
#define ERASE       0x42
#define G_ERASE     0x43
#define RD_APC      0x44
#define WR_APC1     0x45
#define WR_APC2     0x65
#define WR_NVCFG    0x46
#define LD_NVCFG    0x47
#define FWD         0x48
#define CHK_MEM     0x49
#define EXTCLK      0x4A

#define SET_PLAY    0x80
#define SET_REC     0x81
#define SET_ERASE   0x82

#define ISD_ZERO    0x00
#define ISD_LED     0x10

#endif // _ISD1760_H_
