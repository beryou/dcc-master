

extern void  Clear_LCDRegion(unsigned char Left_CorX,Left_CorYp,Right_CorX,nPage);
extern void	 LCD_init(void);
extern void  lcd_xt_start(void);
extern void  PrintfData(uchar idata RowAddr, uchar idata ConlumnAddr,uchar idata DDtat);
extern void  PrintfHz(uchar idata RowAddr, uchar idata ConlumnAddr,uchar  *pchar);
extern void  PrintfChar(uchar idata RowAddr, uchar idata ConlumnAddr,uchar *pchar);