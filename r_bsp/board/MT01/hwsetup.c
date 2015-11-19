/***********************************************************************************************************************
* File Name    : hwsetup.c
* Device(s)    : RX
* H/W Platform : MT01
* Description  : Defines the initialization routines used each time the MCU is restarted.
***********************************************************************************************************************/
/***********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 26.09.2015 1.00     First Release
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* I/O Register and board definitions */
#include "platform.h"

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/
/* MCU I/O port configuration function declaration */
static void output_ports_configure(void);

/* Interrupt configuration function declaration */
static void interrupts_configure(void);

/* MCU peripheral module configuration function declaration */
static void peripheral_modules_enable(void);


/***********************************************************************************************************************
* Function name: hardware_setup
* Description  : Contains setup functions called at device restart
* Arguments    : none
* Return value : none
***********************************************************************************************************************/
void hardware_setup(void)
{
    output_ports_configure();
    interrupts_configure();
    peripheral_modules_enable();
    bsp_non_existent_port_init();
}

/***********************************************************************************************************************
* Function name: output_ports_configure
* Description  : Configures the port and pin direction settings, and sets the pin outputs to a safe level.
* Arguments    : none
* Return value : none
***********************************************************************************************************************/
static void output_ports_configure(void)
{
    /* Unlock MPC registers to enable writing to them. */
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_MPC);
    
    MSTP(EDMAC) = 0 ;                   /* Power up ethernet block */

    /* Port 0 - not used*/
    PORT0.PODR.BYTE = 0x00 ;    /* All outputs low to start */
    PORT0.PDR.BYTE  = 0xFF ;    /* all  are outputs */

    /* Port 1 - UART debug and USB 	VBUS and USB Overcurrent */
    PORT1.PMR.BYTE = 0x5C;
    MPC.P12PFS.BYTE = 0x0A;
	MPC.P13PFS.BYTE = 0x0A;
	MPC.P14PFS.BYTE = 0x12; /* USB0_OVRCURA */
	MPC.P16PFS.BYTE = 0x12; /* USB0_VBUS */
	PORT1.PODR.BYTE = 0x00 ; /* All outputs low to start */
    PORT1.PDR.BYTE  = 0xFB ;  /* All  outputs */
    
    /* Port 2 - USB control  */
    PORT2.PODR.BYTE = 0x00 ;    /* All outputs low to start */
    PORT2.PDR.BYTE  = 0xFF ;    /* All outputs  */
    
    /* Port 3 - JTAG */
    PORT3.PODR.BYTE = 0x00 ;    /* All outputs low to start */
    PORT3.PDR.BYTE = 0x0C;
    /* Port 4 -  */
     PORT4.PODR.BYTE = 0x00 ;    /* All outputs low to start */
     PORT4.PDR.BYTE  = 0xFF ;    /* All outputs  */

     /* Port 5 - Keyboard lines */
     PORT5.PODR.BYTE = 0xFF ;    /* All outputs low to start */
     PORT5.PDR.BYTE  = 0x00 ;    /* All inputs */
     PORT5.PCR.BYTE  = 0x3F ;	 /* Pull up */

     /* Port A - serial memory & PWM  */
     PORTA.PMR.BYTE  = 0x5A ;    /* SCI5 & TIOCA2 */
     MPC.PA1PFS.BYTE = 0x0A ;    /* PA1 is SCK */
     MPC.PA3PFS.BYTE = 0x0A ;    /* PA3 is MISO */
     MPC.PA4PFS.BYTE = 0x0A ;    /* PA4 is MOSI */
     MPC.PA6PFS.BYTE = 0x03 ;	/* PA6 is PWM charge*/
     PORTA.PDR.BYTE  = 0x80 ;    /* PA7 is CS */
     PORTA.PODR.BYTE = 0x00 ;    /* */
     PORTA.PDR.BYTE  = 0xF7 ;    /* All outputs except MISO */

 //    /* Port B - LCD Contrast - not used */
     PORTB.PODR.BYTE = 0x00 ;
     PORTB.PDR.BYTE = 0xFF; /* All outputs */

     /* Port C -  LCD SPI signals && Keyboard Col */
     PORTC.PMR.BYTE  = 0x60 ;    /*  */
     MPC.PC5PFS.BYTE = 0x0D ;    /* PC5 is RSPCKA */
     MPC.PC6PFS.BYTE = 0x0D ;    /* PC6 is MOSIA */
     PORTC.PODR.BYTE = 0x00 ;    /* All outputs low to start */
     PORTC.PDR.BYTE  = 0xFF ;    /* All outputs except MISO */

     /* Port D -  CNC signals */
     PORTD.PODR.BYTE = 0x00 ;    /* All outputs low to start  	*/
     PORTD.PDR.BYTE  = 0xF8 ;    /* PD0-PD2 inputs, PD3-PD7 outputs  */

     /* Port E -  CNC signals*/
     PORTE.PODR.BYTE = 0x00 ;    /* All outputs low to start  */
     PORTE.PDR.BYTE  = 0xFF ;    /* All outputs*/

     /* Port J -  No used	*/
     PORTJ.PODR.BYTE = 0x00 ;    /* All outputs low to start  */
     PORTJ.PDR.BYTE  = 0x08 ;    /* All output */

     /* Lock MPC registers. */
     R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_MPC);
}

/***********************************************************************************************************************
* Function name: interrupts_configure
* Description  : Configures interrupts used
* Arguments    : none
* Return value : none
***********************************************************************************************************************/
static void interrupts_configure(void)
{
    /* Add code here to setup additional interrupts */
}

/***********************************************************************************************************************
* Function name: peripheral_modules_enable
* Description  : Enables and configures peripheral devices on the MCU
* Arguments    : none
* Return value : none
***********************************************************************************************************************/
static void peripheral_modules_enable(void)
{
    /* Add code here to enable peripherals used by the application */
}
