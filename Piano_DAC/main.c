// Runs on TM4C123
// Use SysTick interrupts to implement a 4-key digital piano
// Pawan K
// 16 March 2020
// Port B bits 3-0 have the 4-bit DAC
// Port E bits 3-0 have 4 piano keys

#include "tm4c123gh6pm.h"
#include "Sound.h"
#include "Piano.h"
#include "DAC.h"
#include "PLL.h"
unsigned long input, previous=0;

void delay(unsigned long msec);

int main(void) {
			PLL_Init(); // PLL 80 MHz

    // PortE used for piano keys, PortB used for DAC        
    Sound_Init(); // initialize SysTick timer and DAC
    Piano_Init(); // initialize Port E for input keys
    while (1) {
        // input from keys to select tone
        input = Piano_In();
        if (input ^ previous) // if any key pressed
        {

            Sound_Tone(input); // define frequency
        }
        previous = input; // archiving last value to compare with current value
        delay(20); // 20 ms delay to let stabilize switch vibrations

    }
}
// Inputs: Number of msec to delay
// Outputs: None
void delay(unsigned long msec) {
    while (msec > 0) {  // repeat while there are still delay
        int count = 16000;    // about 1ms delay
        while (count > 0)
        {
            count--;
        } // This while loop takes approximately 3 cycles
        msec--;
    }
}

void SystemInit(void) {}; // system function executes before main

