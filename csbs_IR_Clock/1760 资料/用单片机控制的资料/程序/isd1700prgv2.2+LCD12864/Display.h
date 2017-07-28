/*      This is a HEADFILE of the MOBILEPHONE SALER controller programs
		design by: Dengxianhu
		  Version: E0.2
		     Date: 2003.08		nEvent_Emit_Delay=ReUpLoadelay;
		  company: Forentek		Flag_machine=NUM_TREE;													*/	

//=================== PORT I/O DEFINE =========================================//
//define the relate i/o port's function                                        //
//=============================================================================//

	//--- LCD  instruction
	#define		LCD_WI		0x00
	#define		LCD_RBAC	0x01
	#define		LCD_WD		0x02
	#define		LCD_RD		0x03
	#define		LCDBUSY		0x7f
	//--- LCD timering
	#define		LCD_TAS				0x03//0x03//0x05	
	#define		LCD_TDDW			0x02//0x04//0x06		
	#define		LCD_TDSW			0x06//0x0c//0x10	

//------------------------------------------------------------------------------//
// about the LCD display														//
//------------------------------------------------------------------------------//

extern void	LCD_display_control(unsigned char display_case);
extern void	LCD_display(unsigned char Left_DcornX,unsigned char Left_DcornYP,unsigned char nDcol,unsigned char nDpage,unsigned char *DataBase);
extern void LCD_init(void);
extern void Clear_LCDSChip(unsigned char LeftCorX,LeftCorYP,nDataCol,nDataPage);
extern void Clear_LCDRegion(unsigned char Left_CorX,Left_CorYp,Right_CorX,nPage);
extern void	LCD_CHAR_display(unsigned char Left_DcornX,unsigned char Left_DcornYP,unsigned char CX_size,unsigned char CYP_size,char D_Char);
extern void	LCD_String_display(unsigned char Left_DcornX,unsigned char Left_DcornYP,unsigned char CX_size,unsigned char CYP_size,char str_buf[],unsigned char nLen);
extern void	LCD_bl_on(void);
extern void	LCD_bl_off(void);
extern void wrlattice (unsigned char idata data1,unsigned char idata data2);
extern void LCD_delay (unsigned int us);