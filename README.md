# mini2440-uboot_2016.01
mini2440 uboot based on uboot_2016.01

1. boot from nand, relocate twice
2. eth:tftp/ping is OK.
3. nand write/read OK.
4. SD r/w OK.
5. enable LCD display bmp and show console(MINI2440 P35 LCD)
6. enable bootm kernel(should use modified nand part layout uImage, because uboot.bin is larger than 256K in u-boot_2016)
    new kernel nand part layout:

    static struct mtd_partition friendly_arm_default_nand_part[] = {
        [0] = {
            .name   = "supervivi",
            .size   = 0x00100000,  //0x00040000
            .offset = 0,
        },
        [1] = {
            .name   = "param",
            .offset = 0x00100000, //0x00040000
            .size   = 0x00020000,
        },
        [2] = {
            .name   = "Kernel",
            .offset = 0x00120000, //0x00060000
            .size   = 0x00440000, //0x00500000
        },
        [3] = {
            .name   = "root",
            .offset = 0x00560000,
            .size   = 1024 * 1024 * 1024,
        },
        [4] = {
            .name   = "nand",
            .offset = 0x00000000,
            .size   = 1024 * 1024 * 1024,
        }
    };
