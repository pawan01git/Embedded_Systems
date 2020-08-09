// Sound.c
// Runs on LM4F120 or TM4C123, 
// edX lab 13 
// Use the SysTick timer to request interrupts at a particular period.
// Daniel Valvano, Jonathan Valvano
// December 29, 2014
// This routine calls the 4-bit DAC

#include "Sound.h"  
#include "DAC.h"
#include "Piano.h"
#include "tm4c123gh6pm.h" // library for register address

// **************Sound_Init*********************
// Initialize Systick periodic interrupts
// Also calls DAC_Init() to initialize DAC
// Input: none
// Output: none
unsigned long F, INPUT=0, Index =0;
const unsigned char SineWave[32] ={ 8, 9, 11, 12, 13, 14, 14, 15, 15, 15, 14, 14, 13, 12, 11, 9, 8, 7, 5, 4, 3, 2, 2, 1, 1, 1, 2, 2, 3, 4, 5, 7 };
void Sound_Init(void) {
    DAC_Init();          // Port B is DAC
    Index = 0;
    NVIC_ST_CTRL_R = 0;         // disable SysTick during setup
    NVIC_ST_RELOAD_R = 0;// reload value
    NVIC_ST_CURRENT_R = 0;      // any write to current clears it
    NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x20000000; // priority 1      
    NVIC_ST_CTRL_R = 0x0007;  // enable SysTick with core clock and interrupts

}

// **************Sound_Tone*********************
// Change Systick periodic interrupts to start sound output
// Input: interrupt period
//           Units of period are 12.5ns
//           Maximum is 2^24-1
//           Minimum is determined by length of ISR



void Sound_Tone(unsigned long period) {
    // this routine sets the RELOAD register value and starts SysTick
    NVIC_ST_CTRL_R |= 0x02; //enable systick interrupt
    switch (period)
    {

    case 0x00: 	NVIC_ST_CTRL_R &= ~0x02;
        Sound_Off();
        break;
    case 0x01: NVIC_ST_RELOAD_R = 2500; // 1000 Hz
        break;

    case 0x02: NVIC_ST_RELOAD_R = 2700; //925 Hz
        break;

    case 0x04: NVIC_ST_RELOAD_R = 2900; //862 Hz
        break;

    case 0x08: NVIC_ST_RELOAD_R = 3100; //806 Hz
        break;

    case 0x03: NVIC_ST_RELOAD_R = 3300; //757 Hz
        break;

    case 0x05: NVIC_ST_RELOAD_R = 3500; // 714 Hz
        break;

    case 0x06: NVIC_ST_RELOAD_R = 3700; //675 Hz
        break;

    case 0x07: NVIC_ST_RELOAD_R = 3900; //641 Hz
        break;

    case 0x09: NVIC_ST_RELOAD_R = 4100; //601 Hz
        break;
    case 0x0A: NVIC_ST_RELOAD_R = 4300; // 581 Hz
        break;

    case 0x0B: NVIC_ST_RELOAD_R = 4500; // 555 Hz
        break;
    case 0x0C: NVIC_ST_RELOAD_R = 4700;// 531 Hz
        break;
    case 0x0D: NVIC_ST_RELOAD_R = 4900; // 510 Hz
        break;
    case 0x0E: NVIC_ST_RELOAD_R = 5100; //490 Hz
        break;
    case 0x0F: NVIC_ST_RELOAD_R = 5300; // 471 Hz
        break;
    default:  NVIC_ST_CTRL_R &= ~0x02; // disable systick interrupt
        Sound_Off();
        break;

    }

}




// **************Sound_Off*********************
// stop outputing to DAC
// Output: none
void Sound_Off(void) {
    DAC_Out(0);
}

// Interrupt service routine
// called when an interrupt occurs
void SysTick_Handler(void) {
    Index = (Index+1)&0x1F;
    DAC_Out(SineWave[Index]);
}
