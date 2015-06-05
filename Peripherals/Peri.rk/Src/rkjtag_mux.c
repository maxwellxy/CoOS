#include <peri.h>
#include <task.h>
#include <coocox.h>

OS_STK   task_jtagmux_stk[TASK_STK_SIZE];	 	/*!< Stack of 'task_ddr' task.	*/
OS_STK   task_suspend_stk[TASK_STK_SIZE];


void task_jtagmux(void *pdata)
{
    U32 *p;
    p = (U32 *)0x400c0050;

    /* no sdcard*/
    if (*p & 0x1) {
        /*mcu jtag iomux*/
        pGRF_Reg->GRF_GPIO_IOMUX[1].GPIOB_IOMUX = (0xf<<(2+16)) | (0xa<<2);
    }

    for (;;) {
        if (*p & 0x1) {
            /*no sdcard and iomux is sdmmc*/
            if((pGRF_Reg->GRF_GPIO_IOMUX[1].GPIOA_IOMUX & (0x3<<10)) == (0x1<<10))
            {
                /*check no sdcard again and fix iomux to jtag*/
                if (*p & 0x1) {
                    /*iomux-> jtag*/
                    /*jtag_tck*/
                    pGRF_Reg->GRF_GPIO_IOMUX[1].GPIOA_IOMUX = (0x3<<(14+16)) | (0x2<<14);
                    /*jtag_tms*/
                    pGRF_Reg->GRF_GPIO_IOMUX[1].GPIOB_IOMUX = (0x3<<16) | (0x2<<0);
                    /*mcu jtag*/
                    pGRF_Reg->GRF_GPIO_IOMUX[1].GPIOB_IOMUX = (0xf<<(2+16)) | (0xa<<2);
                }
            }
        }
        /*delay 3 second*/
        CoTimeDelay(0,0,3,0);
    }
}

void Create_JtagMux_Task(void)
{
	/* Create Tasks */
	CoCreateTask(task_jtagmux, (void *)0, JTAG_MUX_PRI, &task_jtagmux_stk[TASK_STK_SIZE-1], TASK_STK_SIZE);
}

static void SetMcuToWfiState(void)
{    
    U32 systick_ctl;
    systick_ctl = NVIC_ST_CTRL & 0xf;
    NVIC_ST_CTRL = 0;

    __asm volatile
    (
        //"b .\n"
        "wfi    \n"
    );
    NVIC_ST_CTRL = systick_ctl;
}

void task_suspend(void *pdata)
{
    for (; ;) {
        StatusType err;
        CoPendMail(mboxs[SCPI_CL_SYS], 0, &err);
        //CoPendMail(suspend_mail, 0, &err);
        if (err == E_OK)
        {
            SetMcuToWfiState();
        }
    }
}

void creat_suspend_task(void)
{
//	CoCreateTask(task_jtagmux, (void *)0, JTAG_MUX_PRI, &task_jtagmux_stk[TASK_STK_SIZE-1], TASK_STK_SIZE);

	CoCreateTask(task_suspend, (void *)0, 4, &task_suspend_stk[TASK_STK_SIZE-1], TASK_STK_SIZE);
}