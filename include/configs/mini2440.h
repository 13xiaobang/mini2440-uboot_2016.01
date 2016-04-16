/*
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 * Gary Jennejohn <garyj@denx.de>
 * David Mueller <d.mueller@elsoft.ch>
 *
 * Configuation settings for the SAMSUNG SMDK2410 board.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __CONFIG_H
#define __CONFIG_H

/*
 * High Level Configuration Options
 * (easy to change)
 */
#define CONFIG_S3C24X0		/* This is a SAMSUNG S3C24x0-type SoC */
#define CONFIG_S3C2440		/* specifically a SAMSUNG S3C2440 SoC */
#define CONFIG_MINI2440		/* on a SAMSUNG SMDK2440 Board */
#define CONFIG_S3C2440_NAND_BOOT

#define CONFIG_SYS_TEXT_BASE	0x33A00000


#define CONFIG_SYS_ARM_CACHE_WRITETHROUGH

/* input clock of PLL (the SMDK2410 has 12MHz input clock) */
#define CONFIG_SYS_CLK_FREQ	12000000

#define CONFIG_CMDLINE_TAG	/* enable passing of ATAGs */
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_INITRD_TAG

/*
 * Hardware drivers
 */
 #if 0
#define CONFIG_CS8900		/* we have a CS8900 on-board */
#define CONFIG_CS8900_BASE	0x19000300
#define CONFIG_CS8900_BUS16	/* the Linux driver does accesses as shorts */
#endif

#define CONFIG_NET_MULTI
#define CONFIG_DRIVER_DM9000    1
#define CONFIG_DM9000_NO_SROM   1
#define CONFIG_DM9000_BASE      0x20000300
#define DM9000_IO			CONFIG_DM9000_BASE
#define DM9000_DATA		(CONFIG_DM9000_BASE + 4)


/*
 * select serial console configuration
 */
#define CONFIG_S3C24X0_SERIAL
#define CONFIG_SERIAL1		1	/* we use SERIAL 1 on SMDK2410 */

/************************************************************
 * USB support (currently only works with D-cache off)
 ************************************************************/
#define CONFIG_USB_OHCI
#define CONFIG_USB_OHCI_S3C24XX
#define CONFIG_USB_KEYBOARD
#define CONFIG_USB_STORAGE
#define CONFIG_DOS_PARTITION

/************************************************************
 * RTC
 ************************************************************/
#define CONFIG_RTC_S3C24X0


#define CONFIG_BAUDRATE		115200

/*
 * BOOTP options
 */
#define CONFIG_BOOTP_BOOTFILESIZE
#define CONFIG_BOOTP_BOOTPATH
#define CONFIG_BOOTP_GATEWAY
#define CONFIG_BOOTP_HOSTNAME

/*
 * Command line configuration.
 */
#define CONFIG_CMD_BSP
#define CONFIG_CMD_CACHE
#define CONFIG_CMD_DATE
#define CONFIG_CMD_DHCP
#define CONFIG_CMD_NAND
#define CONFIG_CMD_PING
#define CONFIG_CMD_REGINFO
#define CONFIG_CMD_USB

#define CONFIG_SYS_HUSH_PARSER
#define CONFIG_CMDLINE_EDITING

/* autoboot */
#define CONFIG_BOOTDELAY	5
#define CONFIG_BOOT_RETRY_TIME	-1
#define CONFIG_RESET_TO_RETRY
#define CONFIG_ZERO_BOOTDELAY_CHECK

#define CONFIG_ETHADDR   08:00:3e:26:0a:5b  //开发板MAC地址
#define CONFIG_NETMASK   255.255.255.0
#define CONFIG_IPADDR    192.168.1.10      //开发板IP地址
#define CONFIG_SERVERIP  192.168.1.2      //Linux主机IP地址


#if defined(CONFIG_CMD_KGDB)
#define CONFIG_KGDB_BAUDRATE	115200	/* speed to run kgdb serial port */
#endif

/*
 * Miscellaneous configurable options
 */
#define CONFIG_SYS_LONGHELP		/* undef to save memory */
#define CONFIG_SYS_CBSIZE	256
/* Print Buffer Size */
#define CONFIG_SYS_PBSIZE	(CONFIG_SYS_CBSIZE + \
				sizeof(CONFIG_SYS_PROMPT)+16)
#define CONFIG_SYS_MAXARGS	16
#define CONFIG_SYS_BARGSIZE	CONFIG_SYS_CBSIZE

#define CONFIG_DISPLAY_CPUINFO				/* Display cpu info */

#define CONFIG_SYS_MEMTEST_START	0x30000000	/* memtest works on */
#define CONFIG_SYS_MEMTEST_END		0x33F00000	/* 63 MB in DRAM */

#define CONFIG_SYS_LOAD_ADDR		0x30800000

/* support additional compression methods */
#define CONFIG_BZIP2
#define CONFIG_LZO
#define CONFIG_LZMA

/*-----------------------------------------------------------------------
 * Physical Memory Map
 */
#define CONFIG_NR_DRAM_BANKS	1          /* we have 1 bank of DRAM */
#define PHYS_SDRAM_1		0x30000000 /* SDRAM Bank #1 */
#define PHYS_SDRAM_1_SIZE	0x04000000 /* 64 MB */

#define PHYS_FLASH_1		0x00000000 /* Flash Bank #0 */

#define CONFIG_SYS_FLASH_BASE	PHYS_FLASH_1

/*-----------------------------------------------------------------------
 * FLASH and environment organization
 */

#define CONFIG_SYS_FLASH_CFI
#define CONFIG_FLASH_CFI_DRIVER
#define CONFIG_FLASH_CFI_LEGACY
#define CONFIG_SYS_FLASH_LEGACY_512Kx16
#define CONFIG_FLASH_SHOW_PROGRESS	45

#define CONFIG_SYS_MAX_FLASH_BANKS	1
#define CONFIG_SYS_FLASH_BANKS_LIST     { CONFIG_SYS_FLASH_BASE }
#define CONFIG_SYS_MAX_FLASH_SECT	(19)

#if 0 /*env is not in nor flash*/
#define CONFIG_ENV_ADDR			(CONFIG_SYS_FLASH_BASE + 0x070000)
#define CONFIG_ENV_IS_IN_FLASH
#define CONFIG_ENV_SIZE			0x10000
#endif
#define CONFIG_ENV_IS_IN_NAND
#define CONFIG_ENV_OFFSET      0x100000
#define CONFIG_ENV_SIZE        0x10000 /* Total Size of Environment Sector */


/* allow to overwrite serial and ethaddr */
#define CONFIG_ENV_OVERWRITE

/*
 * Size of malloc() pool
 * BZIP2 / LZO / LZMA need a lot of RAM
 */
#define CONFIG_SYS_MALLOC_LEN	(4 * 1024 * 1024)

#define CONFIG_SYS_MONITOR_LEN	(448 * 1024)
#define CONFIG_SYS_MONITOR_BASE	CONFIG_SYS_FLASH_BASE

/*
 * NAND configuration
 */
#ifdef CONFIG_CMD_NAND
#define CONFIG_NAND_S3C2440
/*#define CONFIG_S3C2440_NAND_HWECC*/
/*#define CONFIG_NAND_S3C2410*/
/*#define CONFIG_SYS_S3C2410_NAND_HWECC*/

#define CONFIG_SYS_MAX_NAND_DEVICE	1
#define CONFIG_SYS_NAND_BASE		0x4E000000
#endif

/*
 * File system
 */
#define CONFIG_CMD_FAT
#define CONFIG_CMD_EXT2
#define CONFIG_CMD_UBI
#define CONFIG_CMD_UBIFS
#define CONFIG_CMD_MTDPARTS
#define CONFIG_MTD_DEVICE
#define CONFIG_MTD_PARTITIONS
#define CONFIG_YAFFS2
#define CONFIG_RBTREE

/*
 * SD Card support
 * */
#define CONFIG_GENERIC_MMC
#define CONFIG_CMD_MMC
#define CONFIG_MMC  1
#define CONFIG_S3C_SDI
#define CONFIG_CMD_FAT
#define CONFIG_DOS_PARTITIONE


/* additions for new relocation code, must be added to all boards */
#define CONFIG_SYS_SDRAM_BASE	PHYS_SDRAM_1
/*
#define CONFIG_SYS_INIT_SP_ADDR	(CONFIG_SYS_SDRAM_BASE + 0x1000 - \
				GENERATED_GBL_DATA_SIZE)
//here , GENERATED_GBL_DATA_SIZE is not necessary to reserve because in board_init_f_mem it will reserve it.
*/
#define CONFIG_SYS_INIT_SP_ADDR	(CONFIG_SYS_SDRAM_BASE + 0x1000)

#define CONFIG_BOARD_EARLY_INIT_F
#define CONFIG_MINI2440_ASM_PRINT
#define CONFIG_CMD_BURN_IMG

#define CONFIG_EXTRA_ENV_SETTINGS \
	"ethaddr=08:00:3e:26:0a:5b\0" \


#define CONFIG_CMD_BMP
#define CONFIG_LCD
#define CONFIG_MINI2440_FB
#define CONFIG_MINI2440_LCD

#define LCD_COLOR16 4
#define LCD_COLOR24 5

/*#define LCD_BPP LCD_COLOR16 */
#define LCD_BPP LCD_COLOR24

#if (LCD_BPP == LCD_COLOR16)
#define CONFIG_BMP_16BPP
#elif (LCD_BPP == LCD_COLOR24)
#define CONFIG_BMP_24BMP
#endif

#define CONFIG_BOOTARGS         "noinitrd root=/dev/mtdblock3 init=/linuxrc console=ttySAC0"
#define CONFIG_MACH_TYPE 1999

#define CONFIG_SYS_CONSOLE_IS_IN_ENV

#define MINI2440_WATCHDOG
#ifdef MINI2440_WATCHDOG
#define CONFIG_USE_IRQ
#define CONFIG_CMD_MINI2440_WATCHDOG
#endif

#if defined(CONFIG_USE_IRQ)
#define CONFIG_STACKSIZE_IRQ	(4 * 1024)
#define CONFIG_STACKSIZE_FIQ	(4 * 1024)
#endif


#endif /* __CONFIG_H */
