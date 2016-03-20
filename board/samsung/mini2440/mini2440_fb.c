#include <config.h>
#include <common.h>
#include <version.h>
#include <stdarg.h>
#include <linux/types.h>
#include <stdio_dev.h>
#include <lcd.h>

#include <asm/io.h>

vidinfo_t panel_info = {
    .vl_col		= 320,
    .vl_row		= 240,
#if LCD_BPP == LCD_COLOR16
    .vl_bpix	= LCD_COLOR16,
#elif LCD_BPP == LCD_COLOR24
     .vl_bpix	= LCD_COLOR24,
#else
#error Mini2440_fb.c not support this LCD_BPP
#endif
};

//LCDCON1 configure
#define CLKVAL 4
#if LCD_BPP == LCD_COLOR16
#define BPPMODE 12
#elif LCD_BPP == LCD_COLOR24
#define BPPMODE 13
#endif
#define PNRMODE 3

//LCDCON2 configure
#define HEIGHT 240
#define VBPD 9
#define VFPD 1
#define VSPW 2

//LCDCON3,4 configure
#define WIDTH 320
#define HBPD 0x5
#define HFPD 0x0c
#define HSPW 0x01

//LCDCON5 configure
#define FRM565	1
#define INVVCLK 1
#define INVLINE 1
#define INVVFRAME 1
#define PWREN 1
#define BSWP 0
#define HWSWP 1

#define S3C2410_LCDCON5_BPP24BL	    (1<<12)
#define S3C2410_LCDCON5_FRM565	    (1<<11)
#define S3C2410_LCDCON5_INVVCLK	    (1<<10)
#define S3C2410_LCDCON5_INVVLINE    (1<<9)
#define S3C2410_LCDCON5_INVVFRAME   (1<<8)
#define S3C2410_LCDCON5_INVVD	    (1<<7)
#define S3C2410_LCDCON5_INVVDEN	    (1<<6)
#define S3C2410_LCDCON5_INVPWREN    (1<<5)
#define S3C2410_LCDCON5_INVLEND	    (1<<4)
#define S3C2410_LCDCON5_PWREN	    (1<<3)
#define S3C2410_LCDCON5_ENLEND	    (1<<2)
#define S3C2410_LCDCON5_BSWP	    (1<<1)
#define S3C2410_LCDCON5_HWSWP	    (1<<0)

#define LCDCON1    (*(volatile unsigned *)0x4d000000) //LCD control 1
#define LCDCON2    (*(volatile unsigned *)0x4d000004) //LCD control 2
#define LCDCON3    (*(volatile unsigned *)0x4d000008) //LCD control 3
#define LCDCON4    (*(volatile unsigned *)0x4d00000c) //LCD control 4
#define LCDCON5    (*(volatile unsigned *)0x4d000010) //LCD control 5
#define LCDSADDR1  (*(volatile unsigned *)0x4d000014) //STN/TFT Frame buffer start address 1
#define LCDSADDR2  (*(volatile unsigned *)0x4d000018) //STN/TFT Frame buffer start address 2
#define LCDSADDR3  (*(volatile unsigned *)0x4d00001c) //STN/TFT Virtual screen address set

#define Low21Bits(n)	((n)&0x1fffff)

//volatile unsigned short Lcd_Buffer[240][320];

#define GPBCON    (*(volatile unsigned *)0x56000010) //Port B control
#define GPBDAT    (*(volatile unsigned *)0x56000014) //Port B data
#define GPBUP     (*(volatile unsigned *)0x56000018) //Pull-up control B

#define GPCCON    (*(volatile unsigned *)0x56000020) //Port C control
#define GPCDAT    (*(volatile unsigned *)0x56000024) //Port C data
#define GPCUP     (*(volatile unsigned *)0x56000028) //Pull-up control C

#define GPDCON    (*(volatile unsigned *)0x56000030) //Port D control
#define GPDDAT    (*(volatile unsigned *)0x56000034) //Port D data
#define GPDUP     (*(volatile unsigned *)0x56000038) //Pull-up control D

#define GPB1_TO_OUT()		(GPBUP &= 0xfffd, GPBCON &= 0xfffffff3, GPBCON |= 0x00000004)
#define GPB1_TO_1()			(GPBDAT |= 0x0002)


void lcd_ctrl_init (void *lcdbase)
{
    GPB1_TO_OUT();
    GPB1_TO_1();

    GPCUP  = 0x00000000;
    GPCCON = 0xaaaa02a9;

    GPDUP  = 0x00000000;
    GPDCON = 0xaaaaaaaa;

    LCDCON1=(CLKVAL<<8)|(PNRMODE<<5)|(BPPMODE<<1);
    LCDCON2=(VBPD<<24)|((HEIGHT-1)<<14)|(VFPD<<6)|(VSPW);
    LCDCON3=(HBPD<<19)|((WIDTH-1)<<8)|(HFPD);
    LCDCON4=(13 <<  8) | (HSPW << 0);
    LCDSADDR1=(((unsigned int)lcdbase>>22)<<21)|Low21Bits((unsigned int)lcdbase>>1);
#if LCD_BPP == LCD_COLOR16
    LCDCON5  = (S3C2410_LCDCON5_FRM565 | S3C2410_LCDCON5_INVVCLK | S3C2410_LCDCON5_INVVFRAME | S3C2410_LCDCON5_INVVLINE | S3C2410_LCDCON5_HWSWP );
    LCDSADDR2=Low21Bits(((unsigned int)lcdbase+(240*320*2))>>1);
#endif
#if LCD_BPP == LCD_COLOR24
    LCDCON5  = (S3C2410_LCDCON5_FRM565 | S3C2410_LCDCON5_INVVCLK | S3C2410_LCDCON5_INVVFRAME | S3C2410_LCDCON5_INVVLINE);
    LCDSADDR2=Low21Bits(((unsigned int)lcdbase+(240*320*4))>>1);
#endif
    LCDSADDR3=(0<<11)|(WIDTH);
}


#define GPGCON    (*(volatile unsigned *)0x56000060) //Port G control
#define GPGDAT    (*(volatile unsigned *)0x56000064) //Port G data
#define GPGUP     (*(volatile unsigned *)0x56000068) //Pull-up control G


static void Lcd_PowerEnable(int powerEnable)
{
    //GPG4 is setted as LCD_PWREN
    GPGUP  = GPGUP | (1<<4); // Pull-up disable
    GPGCON = GPGCON | (3<<8); //GPG4=LCD_PWREN

    //Enable LCD POWER ENABLE Function
    LCDCON5 = (LCDCON5&(~(1<<3)))|(powerEnable<<3);   // PWREN
    //LCDCON5 = LCDCON5&(~(1<<5))|(invpwren<<5);   // INVPWREN
}


void lcd_enable (void)
{
    Lcd_PowerEnable(1);
    LCDCON1|=1<<0;
}


