#include <common.h>
#include <asm/arch/s3c24x0_cpu.h>

//#include <asm/proc-armv/ptrace.h>
#include "common_usb.h"
#include "2440usb.h"
/* Start : add by www.100ask.net */
void (*isr_handle_array[50])(void);
struct s3c24x0_interrupt * intregs;

extern void IsrUsbd(void);
extern void IsrDma2(void);

/************************* Timer ********************************/
static int intCount;

void IsrWatchdog(void);

void ClearPending_my(int bit) 
{
    intregs->srcpnd= bit;
    intregs->intpnd= bit;
}

void Timer_InitEx(void)
{
    intCount=0;	
    intregs->subsrcpnd= (1<<13);
    ClearPending_my(BIT_WDT_AC97/*BIT_WDT*/);
    intregs->intmsk&=~(BIT_WDT_AC97 /*BIT_WDT*/);
    intregs->intsubmsk&= ~(1<<13);
}


void Timer_StartEx(void)
{
    struct s3c24x0_watchdog * const wdtregs = s3c24x0_get_base_watchdog();

    wdtregs->wtcon=((get_PCLK()/1000000-1)<<8)|(0<<3)|(1<<2);	// 16us
    wdtregs->wtdat=0xffff;
    wdtregs->wtcnt=0xffff;   

    // 1/16/(65+1),interrupt enable,reset disable,watchdog enable
    wdtregs->wtcon=((get_PCLK()/1000000-1)<<8)|(0<<3)|(1<<2)|(0<<0)|(1<<5);   
}

unsigned int Timer_StopEx(void)
{
    int count;
    struct s3c24x0_watchdog * const wdtregs = s3c24x0_get_base_watchdog();

    wdtregs->wtcon=((get_PCLK()/1000000-1)<<8);
    intregs->intmsk|=BIT_WDT_AC97; //BIT_WDT;
    intregs->intsubmsk|= (1<<13);

    count=(0xffff-wdtregs->wtcnt)+(intCount*0xffff);
    return ((unsigned int)count*16/1000000);
}


void  IsrWatchdog(void)
{
    intregs->subsrcpnd= (1<<13);
    ClearPending_my(BIT_WDT_AC97 /* BIT_WDT */);
    intCount++;   	
}

int g_TimerIntHappen;
void  IsrTimer4(void)
{
    ClearPending_my(BIT_TIMER4);
    *(volatile int *)&g_TimerIntHappen = 1;
}


void Dummy_isr(void)
{
    printf("Dummy_isr error, interrupt number: %d, INTMSK = 0x%x\n", intregs->intoffset, intregs->intmsk);
    while(1);
}

void Isr_Init(void)
{
    int i = 0;
    intregs = s3c24x0_get_base_interrupt();

    for (i = 0; i < sizeof(isr_handle_array) / sizeof(isr_handle_array[0]); i++ )
    {
        isr_handle_array[i] = Dummy_isr;
    }

    intregs->intmod=0x0;	      // All=IRQ mode
    intregs->intmsk=BIT_ALLMSK;	  // All interrupt is masked.

    //pISR_URXD0=(unsigned)Uart0_RxInt;	
    //rINTMSK=~(BIT_URXD0);   //enable UART0 RX Default value=0xffffffff

    isr_handle_array[ISR_TIMER4_OFT] = IsrTimer4;
    isr_handle_array[ISR_WDT_OFT]  = IsrWatchdog;

#ifdef CONFIG_USB_DEVICE
    isr_handle_array[ISR_USBD_OFT] = IsrUsbd;
    isr_handle_array[ISR_DMA2_OFT] = IsrDma2;
    ClearPending_my(BIT_DMA2);
    ClearPending_my(BIT_USBD);
#endif    
}

extern volatile int GBL_PRINT;
void IRQ_Handle(void)
{
    unsigned long oft = intregs->intoffset;
    struct s3c24x0_gpio * const gpio = s3c24x0_get_base_gpio();
if(GBL_PRINT)
   printf("IRQ_Handle: %d\n", oft);

    //清中断
    if( oft == 4 ) gpio->eintpend= 1<<7;		//EINT4-7合用IRQ4，注意EINTPEND[3:0]保留未用，向这些位写入1可能导致未知结果
    intregs->srcpnd= 1<<oft;	
    intregs->intpnd	= intregs->intpnd;	 

    /* run the isr */
    isr_handle_array[oft]();
}

/* End : add by www.100ask.net */
