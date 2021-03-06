/**
 ******************************************************************************
 * @file      startup.c
 * @author    Coocox
 * @version   V1.0
 * @date      03/09/2011
 * @brief     Cortex M3 Devices Startup code.
 *            This module performs:
 *                - Set the initial SP
 *                - Set the vector table entries with the exceptions ISR address
 *                - Initialize data and bss
 *                - Call the application's entry point.
 *            After Reset the Cortex-M3 processor is in Thread mode,
 *            priority is Privileged, and the Stack is set to Main.
 *******************************************************************************
 */
 
#include <peri.h>
#include "sram.h"

/*----------Stack Configuration-----------------------------------------------*/  
#define STACK_SIZE       0x00000200      /*!< The Stack size suggest using even number   */
__attribute__ ((section(".co_stack")))
unsigned long pulStack[STACK_SIZE];      


/*----------Macro definition--------------------------------------------------*/  
#define WEAK __attribute__ ((weak))           


/*----------Declaration of the default fault handlers-------------------------*/  
/* System exception vector handler */
__attribute__ ((used))
void WEAK  Reset_Handler(void);   
void WEAK  NMI_Handler(void);       
void WEAK  HardFault_Handler(void); 
void WEAK  MemManage_Handler(void); 
void WEAK  BusFault_Handler(void);  
void WEAK  UsageFault_Handler(void);
void WEAK  SVC_Handler(void);       
void WEAK  DebugMon_Handler(void);  
void WEAK  PendSV_Handler(void);    
void WEAK  SysTick_Handler(void);   
   


/*----------Symbols defined in linker script----------------------------------*/  
extern unsigned long _sidata;    /*!< Start address for the initialization 
                                      values of the .data section.            */
extern unsigned long _sdata;     /*!< Start address for the .data section     */    
extern unsigned long _edata;     /*!< End address for the .data section       */    
extern unsigned long _sbss;      /*!< Start address for the .bss section      */
extern unsigned long _ebss;      /*!< End address for the .bss section        */      
extern void _eram;               /*!< End address for ram                     */

/* SRAM section definitions from the linker */
extern unsigned long __sram_code_start, __ssram_code_text, __esram_code_text;
extern unsigned long __sram_data_start, __ssram_data, __esram_data;


/*----------Function prototypes-----------------------------------------------*/  
extern int main(void);           /*!< The entry point for the application.    */
extern void dump_regs_info(U32 *stack); /* Dump stack information function */
extern void set_mcujtag_iomux(void);

void Default_Reset_Handler(void);   /*!< Default reset handler                */
static void Default_Handler(void);  /*!< Default exception handler            */

/**
  *@brief The minimal vector table for a Cortex M3.  Note that the proper constructs
  *       must be placed on this to ensure that it ends up at physical address
  *       0x00000000.  
  */
__attribute__ ((used,section(".isr_vector")))
void (* const g_pfnVectors[])(void) =
{	
  /*----------Core Exceptions------------------------------------------------ */
  (void *)&pulStack[STACK_SIZE],     /*!< The initial stack pointer         */
  Reset_Handler,             /*!< Reset Handler                               */
  NMI_Handler,               /*!< NMI Handler                                 */
  HardFault_Handler,         /*!< Hard Fault Handler                          */
  MemManage_Handler,         /*!< MPU Fault Handler                           */
  BusFault_Handler,          /*!< Bus Fault Handler                           */
  UsageFault_Handler,        /*!< Usage Fault Handler                         */
  0,0,0,0,                   /*!< Reserved                                    */
  SVC_Handler,               /*!< SVCall Handler                              */
  DebugMon_Handler,          /*!< Debug Monitor Handler                       */
  0,                         /*!< Reserved                                    */
  PendSV_Handler,            /*!< PendSV Handler                              */
  SysTick_Handler,           /*!< SysTick Handler                             */
  
  /*----------256 External Exceptions-----------------------------------------*/
#ifdef RK3368
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,
  Mbox_IRQHandler,		/*!< 138: Mbox0                               */
  Mbox_IRQHandler,		/*!< 139: Mbox0                               */
  Mbox_IRQHandler,		/*!< 140: Mbox0                               */
  Mbox_IRQHandler,		/*!< 141: Mbox0                               */
#elif RK3366
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  Mbox_IRQHandler,		/*!< 128: Mbox0                               */
  Mbox_IRQHandler,		/*!< 129: Mbox0                               */
  Mbox_IRQHandler,		/*!< 130: Mbox0                               */
  Mbox_IRQHandler,		/*!< 131: Mbox0                               */
#elif RK3399
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0, Mbox_IRQHandler,	/*!< 17: Mbox                                 */
  0,0,0,0,0,0,0,0,0,0,0,0,0,0
#else
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
#endif
};


/**
  * @brief  This is the code that gets called when the processor first
  *         starts execution following a reset event. Only the absolutely
  *         necessary set is performed, after which the application
  *         supplied main() routine is called. 
  * @param  None
  * @retval None
  */
void Default_Reset_Handler(void)
{
  /* Initialize data and bss */
  unsigned long *pulSrc, *pulDest;
  U32 sram_size = MCU_SRAM_SIZE;

  /* Copy the data segment initializers from flash to SRAM */
  pulSrc = &_sidata;

  for(pulDest = &_sdata; pulDest < &_edata; )
  {
    *(pulDest++) = *(pulSrc++);
  }
  
  /* Zero fill the bss segment.  This is done with inline assembly since this
     will clear the value of pulDest if it is not kept in a register. */
#ifdef BUILD_M3
  __asm("  ldr     r0, =_sbss\n"
        "  ldr     r1, =_ebss\n"
        "  mov     r2, #0\n"
        "  .thumb_func\n"
        "zero_loop:\n"
        "    cmp     r0, r1\n"
        "    it      lt\n"
        "    strlt   r2, [r0], #4\n"
        "    blt     zero_loop");
#elif BUILD_M0
  __asm("  ldr    r0, =_sbss  \n"
        "  ldr    r1, =_ebss  \n"
        "  mov    r2, #0      \n"
        "  b      zero_loop   \n"
        "  .thumb_func        \n"
        "fill_zero:           \n"
        "  str      r2, [r0]  \n"
        "  add      r0, r0, #4\n"
        "  .thumb_func        \n"
        "zero_loop:           \n"
        "  cmp		r0, r1    \n"
        "  bcc		fill_zero  ");
#endif

  /* Zero sram segment. */
  pulDest = &__ssram_code_text;
  while (sram_size > 0) {
	  *(pulDest++) = 0;
	  sram_size -= sizeof(*pulDest);
  }

  /* Copy sram code from DDR to SRAM */
  pulDest = &__ssram_code_text;
  pulSrc = &__sram_code_start;
  while(pulDest < &__esram_code_text) {
	  *(pulDest++) = *(pulSrc++);
  }

  /* Copy sram data from DDR to SRAM */
  pulDest = &__ssram_data;
  pulSrc = &__sram_data_start;
  while(pulDest < &__esram_data) {
	  *(pulDest++) = *(pulSrc++);
  }

  /* enable mcu jtag */
  set_mcujtag_iomux();

  /* Call the application's entry point.*/
  main();
}


/**
  *@brief Provide weak aliases for each Exception handler to the Default_Handler. 
  *       As they are weak aliases, any function with the same name will override 
  *       this definition.
  */
#pragma weak Reset_Handler = Default_Reset_Handler
#pragma weak NMI_Handler = Default_Handler     
#pragma weak HardFault_Handler = Default_Handler     
#pragma weak MemManage_Handler = Default_Handler     
#pragma weak BusFault_Handler = Default_Handler      
#pragma weak UsageFault_Handler = Default_Handler    
#pragma weak SVC_Handler = Default_Handler           
#pragma weak DebugMon_Handler = Default_Handler      
#pragma weak PendSV_Handler = Default_Handler        
#pragma weak SysTick_Handler = Default_Handler       
      


/**
  * @brief  This is the code that gets called when the processor receives an 
  *         unexpected interrupt.  This simply enters an infinite loop, 
  *         preserving the system state for examination by a debugger.
  * @param  None
  * @retval None  
  */
static void Default_Handler(void) 
{
    volatile register U32 stack;

    /* First, acquire the stack addr of the exception scene */
#ifdef BUILD_M3
    __asm volatile
    (
        "TST    LR, #4          \n"
        "ITE    EQ              \n"
        "MRSEQ  R0, MSP         \n"
        "MRSNE  R0, PSP         \n"
        "MOV    %0, R0          \n"
        : "=r" (stack)
    );
#elif BUILD_M0
    __asm volatile
    (
        "  MOV    R7, #4          \n"
        "  MOV    R0, LR          \n"
        "  TST    R0, R7          \n"
        "  BNE    NE_OPT          \n"
        "  MRS    R0, MSP         \n"
        "  B      END_OPT         \n"
        "NE_OPT:                  \n"
        "  MRS    R0, PSP         \n"
        "END_OPT:                 \n"
        "  MOV    %0, R0          \n"
        : "=r" (stack)
    );
#endif

    /* Second, dump stack regs information */
    dump_regs_info((U32 *)stack);

    /* Go into an infinite loop. */
    while (1) ;
}

/*********************** (C) COPYRIGHT 2009 Coocox ************END OF FILE*****/
