// main.c
// Runs on TM4C123
// March 5, 2020
// this connection occurs in the USB debugging cable
// U0Rx (PA0) connected to serial port on PC
// U0Tx (PA1) connected to serial port on PC
// Ground connected ground in the USB cable
#include "PLL.h"
#include "UART.h" // library for UART functions 
int main(void){ 
	unsigned long n;
		PLL_Init(); // set frequency to 80 MHz
  UART_Init();  // initialize UART

 // EnableInterrupts();       // needed for TExaS
  UART_OutString("Welcome to UART");
  while(1){
    UART_OutString("\n\rInput:"); // calling UART_OutChar function until end of input string
    n = UART_InUDec(); // input data validation and conversion
    UART_OutString(" UART_OutUDec = "); 
    UART_OutUDec(n);     // Modify input and show output
    UART_OutString(",  UART_OutDistance ~ ");
    UART_OutDistance(n); // calls covert distance and output string function
  }
}

void SystemInit(void) {}; // System specific function executes before main

