#define	rNFCONF		(*(volatile unsigned *)0x4E000000)
#define	rNFCONT		(*(volatile unsigned *)0x4E000004)
#define	rNFCMD		(*(volatile unsigned *)0x4E000008)
#define	rNFADDR		(*(volatile unsigned *)0x4E00000C)
#define rNFDATA8	(*(volatile unsigned char*)0x4E000010)
#define	rNFSTAT		(*(volatile unsigned *)0x4E000020)

#define CMD_READ1			0x00												/* 页读命令周期1	*/
#define CMD_READ2			0x30												/* 页读命令周期2	*/
#define CMD_RESET			0xFF										/* 复位				*/

#define NF_CMD(cmd)			{rNFCMD=(cmd);}					/* 写命令			*/
#define NF_ADDR(addr)		{rNFADDR=(addr);}				/* 写地址			*/
#define NF_RDDATA8()		(rNFDATA8)									/* 读8位数据		*/
#define NF_nFCE_L()			{rNFCONT&=~(1<<1);}				/* 片选使能			*/
#define NF_nFCE_H()			{rNFCONT|=(1<<1);}				/* 片选禁用			*/
#define NF_WAITRB()    		{while(!(rNFSTAT&(1<<1)));}		/* 等待就绪			*/
#define NF_CLEAR_RB()    	{rNFSTAT |= (1<<2);}			/* 清除就绪/忙位	*/
#define NF_DETECT_RB()    	{while(!(rNFSTAT&(1<<2)));}		/* 等待就绪			*/

#define TACLS		1
#define TWRPH0		2
#define TWRPH1		1

void delay(int i)
{
	while(i-->0);
}

void Nand_Init(void)
{
	rNFCONF = (TACLS<<12)|(TWRPH0<<8)|(TWRPH1<<4)|(0<<0);	
	rNFCONT = (1<<4)|(1<<1)|(1<<0);
}

static void Nand_Reset(void)
{
	NF_nFCE_L();						/* 片选使能 		*/
	NF_CLEAR_RB();						/* 清除就绪/忙位 	*/
	NF_CMD(CMD_RESET);					/* 写复位命令		*/
	NF_DETECT_RB();						/* 等待就绪			*/
	NF_nFCE_H();						/* 片选禁用			*/
}

void Nand_ReadPage(const int page, unsigned char * const buffer)
{
	int i;
	
	Nand_Reset();
	
	NF_nFCE_L();
	NF_CLEAR_RB();

	NF_CMD(CMD_READ1);	
	NF_ADDR(0x0);
	NF_ADDR(0x0);
	NF_ADDR(page&0xff);
	NF_ADDR((page>>8)&0xff);
	NF_ADDR((page>>16)&0xff);
	NF_CMD(CMD_READ2);	
		 
	NF_DETECT_RB();

	for (i = 0; i < 2048; i++)	
	{
		buffer[i] =  NF_RDDATA8();
	}
 
	NF_nFCE_H();
}

int nand_read(int start_page, int read_pages, unsigned char *buffer)
{
	int i;

	Nand_Init();
	for(i=0; i<read_pages; i++)
	{
		Nand_ReadPage(start_page, buffer + 2048*i);
		start_page++;
	}
	return 0;
}
