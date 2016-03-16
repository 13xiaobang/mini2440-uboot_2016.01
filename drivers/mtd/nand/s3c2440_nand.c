#include <common.h>

#if 0
#define DEBUGN    printf
#else
#define DEBUGN(x, args ...) {}
#endif

#include <nand.h>
#include <asm/arch/s3c24x0_cpu.h>
#include <asm/io.h>

#define __REGb(x)    (*(volatile unsigned char *)(x))
#define __REGi(x)    (*(volatile unsigned int *)(x))

#define NF_BASE  0x4e000000             //Nand配置寄存器基地址
#define NFCONF   __REGi(NF_BASE + 0x0)  //偏移后还是得到配置寄存器基地址
#define NFCONT   __REGi(NF_BASE + 0x4)  //偏移后得到Nand控制寄存器基地址
#define NFCMD    __REGb(NF_BASE + 0x8)  //偏移后得到Nand指令寄存器基地址
#define NFADDR   __REGb(NF_BASE + 0xc)  //偏移后得到Nand地址寄存器基地址
#define NFDATA   __REGb(NF_BASE + 0x10) //偏移后得到Nand数据寄存器基地址
#define NFMECCD0 __REGi(NF_BASE + 0x14) //偏移后得到Nand主数据区域ECC0寄存器基地址
#define NFMECCD1 __REGi(NF_BASE + 0x18) //偏移后得到Nand主数据区域ECC1寄存器基地址
#define NFSECCD  __REGi(NF_BASE + 0x1C) //偏移后得到Nand空闲区域ECC寄存器基地址
#define NFSTAT   __REGb(NF_BASE + 0x20) //偏移后得到Nand状态寄存器基地址
#define NFSTAT0  __REGi(NF_BASE + 0x24) //偏移后得到Nand ECC0状态寄存器基地址
#define NFSTAT1  __REGi(NF_BASE + 0x28) //偏移后得到Nand ECC1状态寄存器基地址
#define NFMECC0  __REGb(NF_BASE + 0x2C) //偏移后得到Nand主数据区域ECC0状态寄存器基地址

#define NFMECC1  __REGi(NF_BASE + 0x30) //偏移后得到Nand主数据区域ECC1状态寄存器基地址
#define NFSECC   __REGi(NF_BASE + 0x34) //偏移后得到Nand空闲区域ECC状态寄存器基地址
#define NFSBLK   __REGi(NF_BASE + 0x38) //偏移后得到Nand块开始地址
#define NFEBLK   __REGi(NF_BASE + 0x3c) //偏移后得到Nand块结束地址

#define S3C2440_NFCONT_nCE  (1<<1)
#define S3C2440_ADDR_NALE   0x0c
#define S3C2440_ADDR_NCLE   0x08
#define S3C2440_NFCONT_EN          (1<<0)
#define S3C2440_NFCONT_INITECC     (1<<4)
#define S3C2440_NFCONT_nFCE        (1<<1)
#define S3C2440_NFCONT_MAINECCLOCK (1<<5)
#define S3C2440_NFCONF_TACLS(x)    ((x)<<12)

#define S3C2410_NFCONF_TWRPH0(x)   ((x)<<8)
#define S3C2410_NFCONF_TWRPH1(x)   ((x)<<4)

ulong IO_ADDR_W = NF_BASE;

static void s3c2440_hwcontrol(struct mtd_info *mtd, int cmd, unsigned int ctrl)
{
	DEBUGN("hwcontrol(): 0x%02x 0x%02x\n", cmd, ctrl);

	if (ctrl & NAND_CTRL_CHANGE)
	{
		IO_ADDR_W = NF_BASE;

		if (!(ctrl & NAND_CLE))                //要写的是地址
			IO_ADDR_W |= S3C2440_ADDR_NALE;
		if (!(ctrl & NAND_ALE))                //要写的是命令
			IO_ADDR_W |= S3C2440_ADDR_NCLE;

		if (ctrl & NAND_NCE)
			NFCONT &= ~S3C2440_NFCONT_nCE;    //使能nand flash
		else
			NFCONT |= S3C2440_NFCONT_nCE;     //禁止nand flash
	}

	if (cmd != NAND_CMD_NONE)
		writeb(cmd,(void *)IO_ADDR_W);
}

static int s3c2440_dev_ready(struct mtd_info *mtd)
{
	DEBUGN("dev_ready\n");
	return (NFSTAT & 0x01);
}

#ifdef CONFIG_S3C2440_NAND_HWECC
void s3c2440_nand_enable_hwecc(struct mtd_info *mtd, int mode)
{
	DEBUGN("s3c2440_nand_enable_hwecc(%p, %d)\n", mtd, mode);
	#if defined(CONFIG_S3C2440)
	 NFCONT |= S3C2440_NFCONT_INITECC;
	#endif

}

static int s3c2440_nand_calculate_ecc(struct mtd_info *mtd, const u_char *dat,
				      u_char *ecc_code)
{
	ecc_code[0] = NFMECC0;
	ecc_code[1] = NFMECC1;
	ecc_code[2] = NFMECC2;
	DEBUGN("s3c2440_nand_calculate_hwecc(%p,): 0x%02x 0x%02x 0x%02x\n",
		mtd , ecc_code[0], ecc_code[1], ecc_code[2]);

	return 0;
}

static int s3c2440_nand_correct_data(struct mtd_info *mtd, u_char *dat,
				     u_char *read_ecc, u_char *calc_ecc)
{
	if (read_ecc[0] == calc_ecc[0] &&
	    read_ecc[1] == calc_ecc[1] &&
	    read_ecc[2] == calc_ecc[2])
		return 0;

	DEBUGN("s3c2440_nand_correct_data: not implemented\n");
	return -1;
}
#endif


int board_nand_init(struct nand_chip *nand)
{
	u_int32_t cfg;
	u_int8_t tacls, twrph0, twrph1;

	DEBUGN("board_nand_init()\n");

	twrph0 = 1; 
	twrph1 = 1; 
	tacls = 0;

	cfg = (tacls<<12)|(twrph0<<8)|(twrph1<<4);
	NFCONF = cfg;

	cfg = (1<<6)|(1<<4)|(0<<1)|(1<<0);
	NFCONT = cfg;

	/* initialize nand_chip data structure */
	nand->IO_ADDR_R = nand->IO_ADDR_W = (void *)0x4e000010;

	/* read_buf and write_buf are default */
	/* read_byte and write_byte are default */

	/* hwcontrol always must be implemented */
	nand->cmd_ctrl = s3c2440_hwcontrol;

	nand->dev_ready = s3c2440_dev_ready;
	nand->ecc.mode = NAND_ECC_SOFT;
	nand->options = 0;
	return 0;
}
