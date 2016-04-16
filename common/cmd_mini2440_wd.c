#include <common.h>
#include <command.h>
#include <asm/arch/s3c24x0_cpu.h>

int do_wd (cmd_tbl_t *cmdtp, int flag, int argc,char* const argv[])
{
    unsigned long reset = 0;
    unsigned long wtcnt= 0;
	unsigned long wtdat= 0;
    unsigned long enable = 0;
    struct s3c24x0_interrupt * intregs = s3c24x0_get_base_interrupt();
    struct s3c24x0_watchdog * dogregs = s3c24x0_get_base_watchdog();

    if(argc>1)
        enable = simple_strtoul(argv[1], NULL, 16);
    if(argc>2)
        reset = simple_strtoul(argv[2], NULL, 16);
    if(argc>3)
        wtdat = simple_strtoul(argv[3], NULL, 16);
	 if(argc>4)
        wtcnt = simple_strtoul(argv[4], NULL, 16);
    printf("watchdog cmd: enable = %ld, reset=%ld, wtdat=0x%lx, wtcnt=0x%lx\r\n", enable, reset ,wtdat, wtcnt);
    if(enable)
    {

        intregs->srcpnd |= ISR_BIT(ISR_WDT_OFT);
        intregs->intpnd |= ISR_BIT(ISR_WDT_OFT);
        intregs->subsrcpnd |= (1<<13);   //subsrcpnd for wdt
        intregs->intmsk&=~(ISR_BIT(ISR_WDT_OFT) /*BIT_WDT*/);
        intregs->intsubmsk&= ~(1<<13);


        dogregs->wtdat = wtdat;
        dogregs->wtcnt = wtcnt;
        if(reset)
            dogregs->wtcon |=(1<<0);
        dogregs->wtcon |= 0xff00;
        dogregs->wtcon |=(1<<5)|(1<<2);
    }
    else
    {
        dogregs->wtcon &=~(1<<5);
        intregs->srcpnd |= ISR_BIT(ISR_WDT_OFT);
        intregs->intpnd |= ISR_BIT(ISR_WDT_OFT);
        intregs->subsrcpnd |= (1<<13);   //subsrcpnd for wdt
        intregs->intmsk|=ISR_BIT(ISR_WDT_OFT); /*BIT_WDT*/
        intregs->intsubmsk|= (1<<13);
    }
    return 0;
}


U_BOOT_CMD(
        wd, 5,  1,  do_wd,
        "This wd cmd is to set wd",
        "wd [want reset?] [wd time]  , all in hex mode\n"
        );


