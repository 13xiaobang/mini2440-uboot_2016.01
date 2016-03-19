#include <common.h>
#include <command.h>
#include <linux/stddef.h>
#include <malloc.h>
#include <nand.h>

int do_burn_img (cmd_tbl_t *cmdtp, int flag, int argc,char* const argv[])
{
    nand_erase_options_t nand_erase_options;
    // set default:
    size_t offset = 0x0;
    size_t length = 0x40000;
    u_char *buf = (u_char*)0x33000000;
    switch (argc) 
    {
        case 0:
        case 1:
        case 2:
        case 3:
            printf ("Usage:\r\n%s\r\n", cmdtp->usage);
            return CMD_RET_USAGE;
        case 4:
            offset = simple_strtoul(argv[1], NULL, 16);
            buf = (u_char*)simple_strtoul(argv[2], NULL, 16);
            length = simple_strtoul(argv[3], NULL, 16);
            break;
        default:
            break;
    }
    printf("Write to Nand: 0x%x from DDR: 0x%x, Len: 0x%x.\r\n", offset, (unsigned int)buf, length);
    memset(&nand_erase_options, 0, sizeof(nand_erase_options));
    nand_erase_options.length = length;
    nand_erase_options.offset = offset;

    if (nand_erase_opts(&nand_info[0], &nand_erase_options))
    {
        printf("do_burn_uboot: erase nand error\r\n");
        return 1;
    }
    printf("Erase done, writting to nand...\r\n");
    size_t end = offset + length;
    size_t amount_saved = 0;
    size_t blocksize, len;
    u_char *char_ptr;

    blocksize = nand_info[0].erasesize;
    len = min(blocksize, length);

    while (amount_saved < length && offset < end) {
        /*we must comfirm that the top area nand is OK when burn uboot image,
          because we do not judge bad block when
          relocate uboot image.So nand_block_isbad seems useless.*/
        if (nand_block_isbad(&nand_info[0], offset)) {
            offset += blocksize;
        } else {
            char_ptr = &buf[amount_saved];
            if (nand_write(&nand_info[0], offset, &len, char_ptr))
            {
                printf("write error , amount_saved=%d\r\n",amount_saved);
                return 1;
            }
            offset += blocksize;
            amount_saved += len;
        }
    }
    printf("amount_writed = 0x%x, length=0x%x\r\n",amount_saved,length);
    printf("done.\r\n");
    return 0;
}


U_BOOT_CMD(
        burn_img, 4,  1,  do_burn_img,
        "This is to easy burn any image into nand!",
        "burn_img [nand_addr] [ddr_addr] [len]  , all in hex mode\n"
        );


