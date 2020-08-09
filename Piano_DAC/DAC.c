// DAC.c
// Runs on TM4C123, 
// Implementation of the 4-bit digital to analog converter
// Pawan K
// 23 March 2020
// Port B bits 3-0 have the 4-bit DAC

#include "DAC.h"
#include "tm4c123gh6pm.h"

// **************DAC_Init*********************
// Initialize 4-bit DAC PB 0 - 3
// Input: none
// Output: none
void DAC_Init(void) {
    unsigned long volatile delay;
    SYSCTL_RCGC2_R |= 0x12; // activate port B
    delay = SYSCTL_RCGC2_R;    // allow time for PortB clock to stabilize
    GPIO_PORTB_DR8R_R |= 0x0F; // internal resistance to limit current
    GPIO_PORTB_AMSEL_R &= ~0x0F;      // disbale analog function
    GPIO_PORTB_PCTL_R &= ~0x0000FFFF; // regular function
    GPIO_PORTB_DIR_R |= 0x0F;      // make PB3-0 out
    GPIO_PORTB_AFSEL_R &= ~0x0F;   // disable alt funct on PB3-0
    GPIO_PORTB_DEN_R |= 0x0F;      // enable digital I/O on PB3-0
}

// **************DAC_Out*********************
// output to DAC
// Input: 4-bit data, 0 to 15 
// Output: none
void DAC_Out(unsigned long data) {

    DAC_Init();
    GPIO_PORTB_DATA_R = data;
}
