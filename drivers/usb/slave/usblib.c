/****************************************************************
NAME: usblib.c
DESC: S3C2440X USB library functions
HISTORY:
Mar.25.2002:purnnamu: ported for S3C2410X.
Mar.27.2002:purnnamu: DMA is enabled.
 ****************************************************************/
#include <common.h>
#include <asm/arch/s3c24x0_cpu.h>

#include "2440usb.h"
#include "usbmain.h"
#include "usblib.h"
#include "usbsetup.h"
#include "usbmain.h"

extern volatile U32 dwUSBBufReadPtr;
extern volatile U32 dwUSBBufWritePtr;
extern volatile U32 dwPreDMACnt;
extern volatile U32 dwNextDMACnt;

extern struct s3c24x0_interrupt* intregs;
extern struct s3c24x0_usb_device* usbdevregs;
extern struct s3c24x0_dmas* dmaregs;

void ConfigUsbd(void)
{
    
        ReconfigUsbd();
        /*
          pISR_USBD =(unsigned)IsrUsbd;
          ClearPending(BIT_USBD);
          rINTMSK&=~(BIT_USBD);  
          */   
        intregs->intmsk&=~(BIT_USBD);  
}


void ReconfigUsbd(void)
{
    // *** End point information ***
    //   EP0: control
    //   EP1: bulk in end point
    //   EP2: not used
    //   EP3: bulk out end point
    //   EP4: not used
    
        usbdevregs->pwr_reg=PWR_REG_DEFAULT_VALUE;	//disable suspend mode
    
        usbdevregs->index_reg=0;	
        usbdevregs->maxp_reg=FIFO_SIZE_8;   	//EP0 max packit size = 8 
    usbdevregs->ep0_csr_in_csr1_reg=EP0_SERVICED_OUT_PKT_RDY|EP0_SERVICED_SETUP_END;	
        //EP0:clear OUT_PKT_RDY & SETUP_END
        usbdevregs->index_reg=1;
#if (EP1_PKT_SIZE==32)
        usbdevregs->maxp_reg=FIFO_SIZE_32;	//EP1:max packit size = 32
#else
    usbdevregs->maxp_reg=FIFO_SIZE_64;	//EP1:max packit size = 64
#endif	
    usbdevregs->ep0_csr_in_csr1_reg=EPI_FIFO_FLUSH|EPI_CDT;	
        usbdevregs->in_csr2_reg=EPI_MODE_IN|EPI_IN_DMA_INT_MASK|EPI_BULK; //IN mode, IN_DMA_INT=masked    
    usbdevregs->out_csr1_reg=EPO_CDT;   	
        usbdevregs->out_csr2_reg=EPO_BULK|EPO_OUT_DMA_INT_MASK;   	
        
        usbdevregs->index_reg=2;
        usbdevregs->maxp_reg=FIFO_SIZE_64;	//EP2:max packit size = 64
    usbdevregs->ep0_csr_in_csr1_reg=EPI_FIFO_FLUSH|EPI_CDT|EPI_BULK;
        usbdevregs->in_csr2_reg=EPI_MODE_IN|EPI_IN_DMA_INT_MASK; //IN mode, IN_DMA_INT=masked    
    usbdevregs->out_csr1_reg=EPO_CDT;   	
        usbdevregs->out_csr2_reg=EPO_BULK|EPO_OUT_DMA_INT_MASK;   	
        
        usbdevregs->index_reg=3;
#if (EP3_PKT_SIZE==32)
        usbdevregs->maxp_reg=FIFO_SIZE_32;	//EP3:max packit size = 32
#else
    usbdevregs->maxp_reg=FIFO_SIZE_64;	//EP3:max packit size = 64
#endif	
    usbdevregs->ep0_csr_in_csr1_reg=EPI_FIFO_FLUSH|EPI_CDT|EPI_BULK;
        usbdevregs->in_csr2_reg=EPI_MODE_OUT|EPI_IN_DMA_INT_MASK; //OUT mode, IN_DMA_INT=masked    
    usbdevregs->out_csr1_reg=EPO_CDT;   	
        //clear OUT_PKT_RDY, data_toggle_bit.
        //The data toggle bit should be cleared when initialization.
        usbdevregs->out_csr2_reg=EPO_BULK|EPO_OUT_DMA_INT_MASK;   	
        
        usbdevregs->index_reg=4;
        usbdevregs->maxp_reg=FIFO_SIZE_64;	//EP4:max packit size = 64
    usbdevregs->ep0_csr_in_csr1_reg=EPI_FIFO_FLUSH|EPI_CDT|EPI_BULK;
        usbdevregs->in_csr2_reg=EPI_MODE_OUT|EPI_IN_DMA_INT_MASK; //OUT mode, IN_DMA_INT=masked    
    usbdevregs->out_csr1_reg=EPO_CDT;   	
        //clear OUT_PKT_RDY, data_toggle_bit.
        //The data toggle bit should be cleared when initialization.
        usbdevregs->out_csr2_reg=EPO_BULK|EPO_OUT_DMA_INT_MASK;   	
        
        usbdevregs->ep_int_reg=EP0_INT|EP1_INT|EP2_INT|EP3_INT|EP4_INT;
        usbdevregs->usb_int_reg=RESET_INT|SUSPEND_INT|RESUME_INT; 
        //Clear all usbd pending bits
        
        //EP0,1,3 & reset interrupt are enabled
        usbdevregs->ep_int_en_reg=EP0_INT|EP1_INT|EP3_INT;
        usbdevregs->usb_int_en_reg=RESET_INT;
        ep0State=EP0_STATE_INIT;
        
}


void RdPktEp0(U8 *buf,int num)
{
    int i;
        
        for(i=0;i<num;i++)
        {
            buf[i]=(U8)usbdevregs->fifo[0].ep_fifo_reg;
        }
}


void WrPktEp0(U8 *buf,int num)
{
    int i;
        
        for(i=0;i<num;i++)
        {
            usbdevregs->fifo[0].ep_fifo_reg=buf[i];	
        }
}


void WrPktEp1(U8 *buf,int num)
{
    int i;
        
        for(i=0;i<num;i++)
        {
            usbdevregs->fifo[1].ep_fifo_reg=buf[i];	
        }
}


void WrPktEp2(U8 *buf,int num)
{
    int i;
        
        for(i=0;i<num;i++)
        {
            usbdevregs->fifo[2].ep_fifo_reg=buf[i];	
        }
}


void RdPktEp3(U8 *buf,int num)
{
    int i;
        
        for(i=0;i<num;i++)
        {
            buf[i]=(U8)usbdevregs->fifo[3].ep_fifo_reg;	
        }
}


void RdPktEp4(U8 *buf,int num)
{
    int i;
        
        for(i=0;i<num;i++)
        {
            buf[i]=(U8)usbdevregs->fifo[4].ep_fifo_reg;	
        }
}


void ConfigEp3DmaMode(U32 bufAddr,U32 count)
{
    int i;
        
        usbdevregs->index_reg=3;
        count=count&0xfffff; //transfer size should be <1MB
    
        dmaregs->dma[2].disrcc=(1<<1)|(1<<0);
        dmaregs->dma[2].disrc=ADDR_EP3_FIFO; //src=APB,fixed,src=EP3_FIFO
    dmaregs->dma[2].didstc=(0<<1)|(0<<0);  
        dmaregs->dma[2].didst=bufAddr;       //dst=AHB,increase,dst=bufAddr
#if USBDMA_DEMAND
    dmaregs->dma[2].dcon=(count)|(0<<31)|(0<<30)|(1<<29)|(0<<28)|(0<<27)|(4<<24)|(1<<23)|(0<<22)|(0<<20); 
        //demand,requestor=APB,CURR_TC int enable,unit transfer,
        //single service,src=USBD,H/W request,autoreload,byte,CURR_TC
#else
        //    rDCON2=(count)|(1<<31)|(0<<30)|(1<<29)|(0<<28)|(0<<27)|(4<<24)|(1<<23)|(0<<22)|(0<<20); 
        /* changed by thisway.diy to disable autoreload */
        dmaregs->dma[2].dcon=(count)|(1<<31)|(0<<30)|(1<<29)|(0<<28)|(0<<27)|(4<<24)|(1<<23)|(1<<22)|(0<<20); 
        //handshake,requestor=APB,CURR_TC int enable,unit transfer,
        //single service,src=USBD,H/W request,autoreload,byte,CURR_TC
#endif        
        dmaregs->dma[2].dmasktrig= (1<<1); 
        //DMA 2 on
        
        //rEP3_DMA_FIFO=0x40; //not needed for OUT operation. 	
        
        usbdevregs->ep3.ep_dma_ttc_l=0xff;
        usbdevregs->ep3.ep_dma_ttc_m=0xff;
        usbdevregs->ep3.ep_dma_ttc_h=0x0f;
        
        usbdevregs->out_csr2_reg=usbdevregs->out_csr2_reg|EPO_AUTO_CLR|EPO_OUT_DMA_INT_MASK; 
        //AUTO_CLR(OUT_PKT_READY is cleared automatically), interrupt_masking.
#if USBDMA_DEMAND
        usbdevregs->ep3.ep_dma_unit=EP3_PKT_SIZE; //DMA transfer unit=64 bytes
    usbdevregs->ep3.ep_dma_con=UDMA_DEMAND_MODE|UDMA_OUT_DMA_RUN|UDMA_DMA_MODE_EN; 
        // deamnd enable,out_dma_run=run,in_dma_run=stop,DMA mode enable
#else        
        usbdevregs->ep3.ep_dma_unit=0x01; //DMA transfer unit=1byte
    usbdevregs->ep3.ep_dma_con=UDMA_OUT_DMA_RUN|UDMA_DMA_MODE_EN;
        // deamnd disable,out_dma_run=run,in_dma_run=stop,DMA mode enable
#endif  
        //wait until DMA_CON is effective.
        usbdevregs->ep3.ep_dma_con;
        for(i=0;i<10;i++);    	
            
                /* add by thisway.diy for non-autoreload */
                dmaregs->dma[3].dmasktrig= (1<<1);
}


void ConfigEp3IntMode(void)
{
    usbdevregs->index_reg=3;
        
        dmaregs->dma[2].dmasktrig= (0<<1);  // EP3=DMA ch 2
    //DMA channel off
    usbdevregs->out_csr2_reg=usbdevregs->out_csr2_reg&~(EPO_AUTO_CLR/*|EPO_OUT_DMA_INT_MASK*/); 
        //AUTOCLEAR off,interrupt_enabled (???)
        usbdevregs->ep3.ep_dma_unit=1;	
        usbdevregs->ep3.ep_dma_con=0; 
        // deamnd disable,out_dma_run=stop,in_dma_run=stop,DMA mode disable
        //wait until DMA_CON is effective.
        usbdevregs->ep3.ep_dma_con;
        
}
