/*
 *  armboot - Startup Code for ARM920 CPU-core
 *
 *  Copyright (c) 2001	Marius Gröger <mag@sysgo.de>
 *  Copyright (c) 2002	Alex Züpke <azu@sysgo.de>
 *  Copyright (c) 2002	Gary Jennejohn <garyj@denx.de>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <asm-offsets.h>
#include <common.h>
#include <config.h>

/*
 *************************************************************************
 *
 * Startup Code (called from the ARM reset exception vector)
 *
 * do important init only if we don't start from memory!
 * relocate armboot to ram
 * setup stack
 * jump to second stage
 *
 *************************************************************************
 */

	.globl	reset

reset:
	/*
	 * set the cpu to SVC32 mode
	 */
	mrs	r0, cpsr
	bic	r0, r0, #0x1f
	orr	r0, r0, #0xd3
	msr	cpsr, r0

#if	defined(CONFIG_AT91RM9200DK) || defined(CONFIG_AT91RM9200EK)
	/*
	 * relocate exception table
	 */
	ldr	r0, =_start
	ldr	r1, =0x0
	mov	r2, #16
copyex:
	subs	r2, r2, #1
	ldr	r3, [r0], #4
	str	r3, [r1], #4
	bne	copyex
#endif

#ifdef CONFIG_S3C24X0
	/* turn off the watchdog */

# if defined(CONFIG_S3C2400)
#  define pWTCON	0x15300000
#  define INTMSK	0x14400008	/* Interrupt-Controller base addresses */
#  define CLKDIVN	0x14800014	/* clock divisor register */
#else
#  define pWTCON	0x53000000
#  define INTMSK	0x4A000008	/* Interrupt-Controller base addresses */
#  define INTSUBMSK	0x4A00001C
#  define CLKDIVN	0x4C000014	/* clock divisor register */
# endif

	ldr	r0, =pWTCON
	mov	r1, #0x0
	str	r1, [r0]

	/*
	 * mask all IRQs by setting all bits in the INTMR - default
	 */
	mov	r1, #0xffffffff
	ldr	r0, =INTMSK
	str	r1, [r0]
# if defined(CONFIG_S3C2410)
	ldr	r1, =0x3ff
	ldr	r0, =INTSUBMSK
	str	r1, [r0]
# endif

# if defined(CONFIG_MINI2440)
#  define LOCKTIME		0x4C000000
#  define MPLLCON			0x4C000004
#  define UPLLCON			0x4C000008

#  define CLKDIV_VAL	5
#  define M_MDIV	0x7f				/* XTal=12.0MHz MPLL=405MHz */
#  define M_PDIV	2
#  define M_SDIV	1
#  define U_MDIV	0x38				/* XTal=12.0MHz UPLL=48MHz */
#  define U_PDIV	2
#  define U_SDIV	2

	ldr	r1, =0x7fff
	ldr	r0, =INTSUBMSK
	str	r1, [r0]

	ldr	r0,=LOCKTIME						/* ÉèÖÃU_LTIMEºÍM_LTIME */
	ldr	r1,=0x0fff0fff
	str	r1,[r0]

	ldr	r0, =CLKDIVN						/* Fclk:Hclk:Pclk = 1:4:8 */
	ldr	r1, =CLKDIV_VAL
	str	r1, [r0]

	mrc	p15, 0, r0, c1, c0, 0			/* ×ÜÏßÄ£Ê½ÉèÎªÒì²½Ä£Ê½ */
	orr	r0, r0, #0xc0000000
	mcr	p15, 0, r0, c1, c0, 0

	ldr	r0,=UPLLCON						/* ÅäÖÃUPLL */
	ldr	r1,=((U_MDIV<<12) + (U_PDIV<<4) + U_SDIV)
	str	r1,[r0]
	nop													/* ÎªÈ·±£Ó²¼þÍê³É²Ù×÷£¬ÖÁÉÙÐè7¸öÊ±ÖÓÖÜÆÚ */
	nop
	nop
	nop
	nop
	nop
	nop

	ldr	r0,=MPLLCON						/* ÅäÖÃMPLL */
	ldr	r1,=((M_MDIV<<12) + (M_PDIV<<4) + M_SDIV)
	str	r1,[r0]
#else
	/* FCLK:HCLK:PCLK = 1:2:4 */
	/* default FCLK is 120 MHz ! */
	ldr	r0, =CLKDIVN
	mov	r1, #3
	str	r1, [r0]
#endif	/* CONFIG_MINI2440 */

#endif	/* CONFIG_S3C24X0 */

	/*
	 * we do sys-critical inits only at reboot,
	 * not when booting from ram!
	 */
#ifndef CONFIG_SKIP_LOWLEVEL_INIT
	bl	cpu_init_crit
#endif

#ifdef CONFIG_MINI2440_ASM_PRINT
	bl 	print_init_log
#endif
	bl	_main

/*------------------------------------------------------------------------------*/

	.globl	c_runtime_cpu_setup
c_runtime_cpu_setup:

	mov	pc, lr

/*
 *************************************************************************
 *
 * CPU_init_critical registers
 *
 * setup important registers
 * setup memory timing
 *
 *************************************************************************
 */
#ifdef CONFIG_MINI2440_ASM_PRINT
print_init_log:

/*init uart io*/
	ldr	r0, =0x56000070
	ldr	r1, =0x002AFAAA
	str	r1, [r0]

	ldr	r0, =0x56000078
	ldr	r1, =0x000007FF
	str	r1, [r0]

#define MINI2440_ULCON0 0x50000000
#define MINI2440_UCON0 0x50000004
#define MINI2440_UFCON0 0x50000008
#define MINI2440_UMCON0 0x5000000c
#define MINI2440_UBRDIV0 0x50000028

/*init uart controller*/
	ldr r0,=MINI2440_ULCON0
	ldr r1,=0x3
	str r1,[r0]

	ldr r0,=MINI2440_UCON0
	ldr r1,=0x245
	str r1,[r0]

	ldr r0,=MINI2440_UFCON0
	ldr r1,=0x1
	str r1,[r0]

	ldr r0,=MINI2440_UMCON0
	ldr r1,=0x0
	str r1,[r0]

	ldr r0,=MINI2440_UBRDIV0
	ldr r1,=0x1a
	str r1,[r0]

/*send char to tx buffer*/
	ldr	r0, =0x50000020

	ldr	r1,='T'
	str r1, [r0]

	ldr	r1, ='h'
	str r1, [r0]

	ldr	r1, ='i'
	str r1, [r0]

	ldr	r1, ='s'
	str r1, [r0]

	ldr	r1, =' '
	str r1, [r0]

	ldr	r1, ='i'
	str r1, [r0]

	ldr r1, ='s'
	str r1, [r0]

	ldr	r1, =' '
	str r1, [r0]

	ldr	r1, ='l'
	str r1, [r0]

	ldr	r1, ='o'
	str r1, [r0]

	ldr	r1, ='g'
	str r1, [r0]

	ldr	r1, ='.'
	str r1, [r0]

	mov pc, lr
#endif

#ifndef CONFIG_SKIP_LOWLEVEL_INIT
cpu_init_crit:
	/*
	 * flush v4 I/D caches
	 */
	mov	r0, #0
	mcr	p15, 0, r0, c7, c7, 0	/* flush v3/v4 cache */
	mcr	p15, 0, r0, c8, c7, 0	/* flush v4 TLB */

	/*
	 * disable MMU stuff and caches
	 */
	mrc	p15, 0, r0, c1, c0, 0
	bic	r0, r0, #0x00002300	@ clear bits 13, 9:8 (--V- --RS)
	bic	r0, r0, #0x00000087	@ clear bits 7, 2:0 (B--- -CAM)
	orr	r0, r0, #0x00000002	@ set bit 2 (A) Align
	orr	r0, r0, #0x00001000	@ set bit 12 (I) I-Cache
	mcr	p15, 0, r0, c1, c0, 0

	/*
	 * before relocating, we have to setup RAM timing
	 * because memory timing is board-dependend, you will
	 * find a lowlevel_init.S in your board directory.
	 */
	mov	ip, lr

	bl	lowlevel_init

	mov	lr, ip
	mov	pc, lr
#endif /* CONFIG_SKIP_LOWLEVEL_INIT */
