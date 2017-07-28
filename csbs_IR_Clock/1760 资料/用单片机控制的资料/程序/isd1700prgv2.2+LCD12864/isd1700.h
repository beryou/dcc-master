#define ISD1700_PU          0x01
#define ISD1700_STOP        0X02
#define ISD1700_REST        0x03
#define ISD1700_CLR_INT     0x04
#define ISD1700_RD_STAUS    0x05
#define ISD1700_RD_PLAY_PTR 0x06
#define ISD1700_PD          0x07
#define ISD1700_RD_REC_PTR  0x08
#define ISD1700_DEVID       0x09

#define ISD1700_PLAY        0x40
#define ISD1700_REC         0x41
#define ISD1700_ERASE       0x42
#define ISD1700_G_ERASE     0x43
#define ISD1700_RD_APC      0x44
#define ISD1700_WR_APC1     0x45
#define ISD1700_WR_APC2     0x65

#define ISD1700_WR_NVCFG    0x46
#define ISD1700_LD_NVCFG    0x47
#define ISD1700_FWD         0x48
#define ISD1700_CHK_MEM     0x49
#define ISD1700_EXTCLK      0x4A
#define ISD1700_SET_PLAY    0x80
#define ISD1700_SET_REC     0x81
#define ISD1700_SET_ERASE   0x82
#define NULL                0x00
#define ISD_LED             0x10



extern unsigned char data ISD_COMM_RAM_C[7];

extern void  init(void);
extern void  delay(int x);
extern void  comm_sate(void);
extern void  rest_isd_comm_ptr(void);
extern unsigned char T_R_comm_byte(unsigned char comm_data );
extern void  isd1700_par2_comm(unsigned char comm_par, unsigned int data_par);
extern void  isd1700_Npar_comm(unsigned char comm_par,comm_byte_count); 
extern void  isd1700_7byte_comm(unsigned char comm_par, unsigned int star_addr, unsigned int end_addr);

extern void  spi_pu (void);
extern void  spi_stop (void);
extern void  spi_Rest ( void );
extern void  spi_CLR_INT(void);
extern void  spi_RD_STAUS(void);
extern void  spi_RD_play_ptr(void);
extern void  spi_pd(void);
extern void  spi_RD_rec_ptr(void);
extern void  spi_devid(void);
extern void  spi_play(void);
extern void  spi_rec (void);
extern void  spi_erase (void);
extern void  spi_G_ERASE (void);
extern void  spi_rd_apc(void);
extern void  spi_wr_apc1 (void);
extern void  spi_wr_apc2 (void);
extern void  spi_wr_nvcfg (void);
extern void  spi_ld_nvcfg (void);
extern void  spi_fwd (void);
extern void  spi_chk_mem(void);
extern void  spi_CurrRowAddr(void);

extern void  seril_back_sate(unsigned char byte_number);
extern void  spi_set_opt(unsigned char spi_set_comm);

