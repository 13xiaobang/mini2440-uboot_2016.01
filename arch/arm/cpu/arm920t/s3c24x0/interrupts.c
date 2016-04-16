/*
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Alex Zuepke <azu@sysgo.de>
 *
 * (C) Copyright 2002
 * Gary Jennejohn, DENX Software Engineering, <gj@denx.de>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>

#include <asm/arch/s3c24x0_cpu.h>
#include <asm/proc-armv/ptrace.h>

void (*isr_handle_array[32])(void);

static void dummy_irq(void)
{
    printf("go into dummy irq, halt!\r\n");
    for(;;);
}

static void isr_watchdog(void)
{
    struct s3c24x0_interrupt * intregs = s3c24x0_get_base_interrupt();
    struct s3c24x0_watchdog* dogregs  = s3c24x0_get_base_watchdog();

    dogregs->wtcon &=~((1<<5)|(1<<3));
    intregs->intmsk|=ISR_BIT(ISR_WDT_OFT); /*BIT_WDT*/
    intregs->intsubmsk|= (1<<13);

    intregs->srcpnd = ISR_BIT(ISR_WDT_OFT);
    intregs->intpnd = ISR_BIT(ISR_WDT_OFT);
    intregs->subsrcpnd= (1<<13);   //subsrcpnd for wdt
    printf("watchdog irq served.\r\n");
}

void isr_init()
{
    int i;
    struct s3c24x0_interrupt * intregs = s3c24x0_get_base_interrupt();

    for (i = 0; i < sizeof(isr_handle_array) / sizeof(isr_handle_array[0]); i++ )
    {
        isr_handle_array[i] = dummy_irq;
    }

    intregs->intmod=0x0;		// All=IRQ mode
    intregs->intmsk=BIT_ALLMSK;// All interrupt is masked.
    isr_handle_array[ISR_WDT_OFT] = isr_watchdog;
}

void do_irq (struct pt_regs *pt_regs)
{
    struct s3c24x0_interrupt * intregs = s3c24x0_get_base_interrupt();

    unsigned long oft = intregs->intoffset;
    //   if(GOLBAL_DBG_PRINT)
    //printf("IRQ_Handle: %d\n", oft);
    intregs->srcpnd = 1<<oft;
    intregs->intpnd	=1<<oft;

    /* run the isr */
    isr_handle_array[oft]();
}

#ifdef CONFIG_USE_IRQ
int arch_interrupt_init(void)
{
    isr_init();
}
#endif
