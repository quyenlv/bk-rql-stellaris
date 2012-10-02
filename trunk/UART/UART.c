//*****************************************************************************
//
// uart_echo.c - Example for reading data from and writing data to the UART in
//               an interrupt driven fashion.
//
// Copyright (c) 2008-2012 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 9107 of the DK-LM3S9B96 Firmware Package.
//
//*****************************************************************************
#include "inc/lm3s9790.h"
#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "grlib/grlib.h"
#include "glcd.h"
#include "utils/uartstdio.h"
#include "driverlib/adc.h"

//Global Variable
const char keyPadMatrix[] = 
{ 
    '1','2','3','U',
    '4','5','6','D',
    '7','8','9','<',
    'T','0','C','@',
    0xFF
};

char ScanKeyMatrix()
{
    // This routine returns the first key found to be
    // pressed during the scan.
    char key = 0;
	int row;
	
    for( row = 0x10; row < 0x100; row <<= 1 )
    {     
        {   // turn on row output
            //row1port = row.0;
            //row2port = row.1;
            //row3port = row.2;
            //row4port = row.3;
			GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7,row);
        }   

        // read colums - break when key press detected
		
        if( GPIOPinRead( GPIO_PORTE_BASE, GPIO_PIN_0 ) )
            break;
        key++;
        if( GPIOPinRead( GPIO_PORTE_BASE, GPIO_PIN_1 ) )
            break;
        key++;
        if( GPIOPinRead( GPIO_PORTE_BASE, GPIO_PIN_2 ) )
            break;
        key++;
		if( GPIOPinRead( GPIO_PORTE_BASE, GPIO_PIN_3 ) )
            break;
        key++;
    }
	
	GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7,0);
        
    return keyPadMatrix[key]; 
}

//*****************************************************************************
//
//! \addtogroup example_list
//! <h1>UART Echo (uart_echo)</h1>
//!
//! This example application utilizes the UART to echo text.  The first UART
//! (connected to the FTDI virtual serial port on the evaluation board) will be
//! configured in 115,200 baud, 8-n-1 mode.  All characters received on the
//! UART are transmitted back to the UART.
//
//*****************************************************************************

char i,j;
//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif

//DELAY FUNCTION
unsigned long g_ulCounter = 0;
void
SysTickIntHandler(void)
{
    // Update the Systick interrupt counter.
    g_ulCounter++;
}

//this called from outside 
void delay_ms (unsigned int milliseconds)
{ 
   // Initialize the interrupt counter.
   g_ulCounter = 0;

   // Set up the period for the SysTick timer.  The SysTick timer period will
   // be equal to the system clock, resulting in a period of 1 second.
   //here I will divide by 1000 so Interrupt at 1 ms
   ROM_SysTickPeriodSet(ROM_SysCtlClockGet()/1000);

   // Enable interrupts to the processor.
   ROM_IntMasterEnable();

   // Enable the SysTick Interrupt.
   ROM_SysTickIntEnable();

   // Enable SysTick.
   ROM_SysTickEnable();

   // Loop  while the SysTick count g_ulCounter is less than milliseconds.
   while(g_ulCounter < milliseconds);

} 

//*****************************************************************************
//
// The UART interrupt handler.
//
//*****************************************************************************
void
UARTIntHandler(void)
{
    unsigned long ulStatus;
	volatile unsigned long ulLoop;
    //
    // Get the interrrupt status.
    //
    ulStatus = ROM_UARTIntStatus(UART0_BASE, true);

    //
    // Clear the asserted interrupts.
    //
    ROM_UARTIntClear(UART0_BASE, ulStatus);

    //
    // Loop while there are characters in the receive FIFO.
    //
    while(ROM_UARTCharsAvail(UART0_BASE))
    {
		switch(UARTCharGetNonBlocking(UART0_BASE)){
			case 'A':
				//Start pump
				GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_7,~GPIO_PIN_7);
				break;
			case 'F':
				//Turn off pump
				GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_7,GPIO_PIN_7);	
				break;
			case 'S':
				//Turn off pump for waiting Hemoclean
				GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_7,GPIO_PIN_7);		
				break;
		}
		ROM_UARTCharPutNonBlocking(UART1_BASE,UARTCharGetNonBlocking(UART0_BASE));
    }
}

//*****************************************************************************
//
// Send a string to the UART.
//
//*****************************************************************************
void
UARTSend(const unsigned char *pucBuffer, unsigned long ulCount)
{
    //
    // Loop while there are more characters to send.
    //
    while(ulCount--)
    {
        //
        // Write the next character to the UART.
        //
        ROM_UARTCharPut(UART1_BASE, *pucBuffer++);
    }
}

//*****************************************************************************
//
// This example demonstrates how to send a string of data to the UART.
//
//*****************************************************************************
int
main(void)
{
	volatile unsigned long ulLoop;
	char key_return;
	int fuck = 3;
	int row = 2;
    //
	// Set the clocking to run directly from the internal crystal/oscillator.
	//
	ROM_SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_INT | SYSCTL_MAIN_OSC_DIS |
		SYSCTL_XTAL_16MHZ);
		
	// Enable IO port for UARTs
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE,GPIO_PIN_7);
	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);
	GPIOPinConfigure(GPIO_PD2_U1RX);
	GPIOPinConfigure(GPIO_PD3_U1TX);
	
	//Matrix keypad configure
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE,GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7); //Configure ROWs are output

	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	GPIOPinTypeGPIOInput(GPIO_PORTE_BASE,GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3); //Configure COLUMNs are input	
	GPIOPadConfigSet(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3,
			GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    //
    // Enable the (non-GPIO) peripherals used by this example. 
    // already enabled GPIO Port A.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);

    //
    // Enable processor interrupts.
    //
    IntMasterEnable();

    //
    // Set UART pins.
    //
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	ROM_GPIOPinTypeUART(GPIO_PORTD_BASE, GPIO_PIN_2 | GPIO_PIN_3);
    //
    // Configure UARTs for 9600, 8-N-1 operation.
    //
    ROM_UARTConfigSetExpClk(UART0_BASE, ROM_SysCtlClockGet(), 9600,
                            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                             UART_CONFIG_PAR_NONE));
	ROM_UARTConfigSetExpClk(UART1_BASE, ROM_SysCtlClockGet(), 9600,
                            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                             UART_CONFIG_PAR_NONE));

    //
    // Enable UARTs interrupt.
    //
    ROM_IntEnable(INT_UART0);
    ROM_UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
	ROM_UARTIntEnable(UART1_BASE, UART_INT_RX | UART_INT_RT);
   	
	//
    // Initialize the display driver.
    //
    GLCD_INIT();
	GLCD_FILL(0);
    GLCD_DRW_REC_SOLID(45,45,20,20,1);
	//GLCD_CHAR_SET(0, 0, basic_font, 'A');
	//GLCD_OUT_STR(40,1,"Minh THu MT",1);
	//GLCD_DRW_LINE(2,2,8,8,1);
	//GLCD_OUT_DEC(60, 0 ,1234 ,4, 1);
    GLCD_DISPLAY();
	//ROM_UARTCharPutNonBlocking(UART1_BASE,fuck);
	UARTSend((unsigned char *)fuck, 1);
	//Send notification
	UARTSend((unsigned char *)"Automated Dialyzer and Bloodline Washing System", 47);

	//GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7,0x20);
	//while(1);
	UARTStdioInit(1);
	UARTprintf("\nuDMA/ADC Test %d",4664);
	while(1)
	{
		 key_return = ScanKeyMatrix();
		 if(key_return != 0xFF)
		 	ROM_UARTCharPutNonBlocking(UART1_BASE,key_return);
	}
	while(1)
	{
		if( GPIOPinRead( GPIO_PORTE_BASE, GPIO_PIN_3 ) )
		{
			SysCtlDelay(500000);	//delay 40ms	
			if( GPIOPinRead( GPIO_PORTE_BASE, GPIO_PIN_3 ) )			
				UARTSend((unsigned char *)"\nPin 3 is turned on", 19);
		}
	}
	while(1)
	{
		UARTCharPut(UART0_BASE, 'A');
		for(ulLoop = 0; ulLoop < 10000000; ulLoop++)
        {
        }
	}

}