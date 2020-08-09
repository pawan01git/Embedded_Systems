// Piano.c
// Runs on TM4C123, 
// There are four keys in the piano (PortE pins 0-3)
// Pawan K
// 15 March 2020

#include "Piano.h"
#include "tm4c123gh6pm.h"


// **************Piano_Init*********************
// Initialize piano key inputs
// Input: none
// Output: none

void Piano_Init(void) {
    volatile unsigned long  delay; // a variable
    SYSCTL_RCGC2_R |= 0x00000010;  //activate clock for Port F
    delay = SYSCTL_RCGC2_R;           // allowing time for clock to stabilise
    GPIO_PORTE_AMSEL_R = 0x00;        // disable analog on PE 3-0
    GPIO_PORTE_PCTL_R &= ~0x0000FFFF;   // PCTL GPIO on PE3-0
    GPIO_PORTE_DIR_R &= ~0x0F;          // PE3-0 as input
    GPIO_PORTE_AFSEL_R &= ~0x0F;        // disable alt funct on PF7-0
    GPIO_PORTE_PDR_R |= 0x0F;          // enable pull-up on PF0 and PF4
    GPIO_PORTE_DEN_R |= 0x0F;          // enable digital I/O on PF4-0

}



// **************Piano_In*********************
// Input from piano key inputs
// Input: none 
// Output: 0 to 15 depending on keys
// 0x01 is key 0 pressed, 0x02 is key 1 pressed,
// 0x04 is key 2 pressed, 0x08 is key 3 pressed
unsigned long Piano_In(void) {
    return (GPIO_PORTE_DATA_R&0x0F);

}

