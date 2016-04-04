/*
 *  relocate - common relocation function for ARM U-Boot
 *
 *  Copyright (c) 2013  Albert ARIBAUD <albert.u.boot@aribaud.net>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <asm-offsets.h>
#include <config.h>
#include <linux/linkage.h>
#ifdef CONFIG_CPU_V7M
#include <asm/armv7m.h>
#endif

#ifdef CONFIG_S3C2440_NAND_BOOT
.globl _TEXT_BASE
_TEXT_BASE:
	.word	CONFIG_SYS_TEXT_BASE
#endif

/*
 * void relocate_code(addr_moni)
 *
 * This function relocates the monitor code.
 *
 * NOTE:
 * To prevent the code below from containing references with an R_ARM_ABS32
 * relocation record type, we never refer to linker-defined symbols directly.
 * Instead, we declare literals which contain their relative location with
 * respect to relocate_code, and at run time, add relocate_code back to them.
 */

ENTRY(first_relocate_code)
#ifdef CONFIG_S3C2440_NAND_BOOT
	ldr    r0, = __image_copy_start
	ldr 	r1, =__image_copy_end
	sub  r1, r1, r0
	mov	r1, r1, LSR #11
	add 	r1, r1, #1
	ldr 	r0, =0x0;
	ldr 	r2, =(CONFIG_SYS_TEXT_BASE)

	push {lr} // save the temp reg
	push {r2}

	bl	nand_read
	pop {r4}
	pop {lr}
	ldr 	pc, =relocations
#endif

relocations:
	/*
	 * fix .rel.dyn relocations
	 */
	ldr	r2, =__rel_dyn_start	/* r2 <- SRC &__rel_dyn_start */
	ldr	r3, =__rel_dyn_end	/* r3 <- SRC &__rel_dyn_end */
fixloop:
	ldmia	r2!, {r0-r1}		/* (r0,r1) <- (SRC location,fixup) */
	and	r1, r1, #0xff
	cmp	r1, #23			/* relative fixup? */
	bne	fixnext

	/* relative fix: increase location by offset */
	add	r0, r0, r4
	ldr	r1, [r0]
	add	r1, r1, r4
	str	r1, [r0]
fixnext:
	cmp	r2, r3
	blo	fixloop

relocate_done:

#ifdef __XSCALE__
	/*
	 * On xscale, icache must be invalidated and write buffers drained,
	 * even with cache disabled - 4.2.7 of xscale core developer's manual
	 */
	mcr	p15, 0, r0, c7, c7, 0	/* invalidate icache */
	mcr	p15, 0, r0, c7, c10, 4	/* drain write buffer */
#endif

	/* ARMv4- don't know bx lr but the assembler fails to see that */

#ifdef __ARM_ARCH_4__
	mov	pc, lr
#else
	bx	lr
#endif

ENDPROC(first_relocate_code)
