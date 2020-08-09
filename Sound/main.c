// Sound gernation through Systick Interrupt
// Runs on TM4C123
// Use SysTick interrupts to create a squarewave at 880 Hz.
// There is a positive logic switch connected to PA3
// There is an output on PA2
//   connected to headphones through a 1k resistor.
// The volume-limiting resistor can be any value from 680 to 2000 ohms
// The tone is initially off, when the switch goes from
// not touched to touched, the tone toggles on/off. (rising edge triggered)
//                   |---------|               |---------|
// Switch   ---------|         |---------------|         |------
//
//                    |-| |-| |-| |-| |-| |-| |-|
// Tone     ----------| |-| |-| |-| |-| |-| |-| |---------------
//
// Pawan K
// 10 March 2020

#include "tm4c123gh6pm.h" // library for register address
#include "PLL.h" // library for PLL register and function


// Port A initialization as PA2 output

void Sound_Init(void)
{
    SYSCTL_RCGC2_R |= 0x00000001;                                  // activate clock for port A
    GPIO_PORTA_AMSEL_R &= ~0x04;                                   // disable analog functionality on PA2
    GPIO_PORTA_PCTL_R &= ~0x00000F00;                              // configure PA5 as GPIO
    GPIO_PORTA_DIR_R |= 0x04;                                      // make PA2 out
    GPIO_PORTA_DR8R_R |= 0x04;                                     // enable 8 mA drive on PA2
    GPIO_PORTA_AFSEL_R &= ~0x04;                                   // disable alt funct on PA2
    GPIO_PORTA_DEN_R |= 0x04;                                      // enable digital I/O on PA2
    GPIO_PORTA_DATA_R &= ~0x04;                                    // make PA2 low
    NVIC_ST_CTRL_R = 0;                                            // disable SysTick during setup
    NVIC_ST_RELOAD_R = 90908;                                      // reload value for 880 Hz
    NVIC_ST_CURRENT_R = 0;                                         // any write to current clears it
    NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R & 0x00FFFFFF) | 0x40000000; // priority 2
    NVIC_ST_CTRL_R = 0x00000007;                                   // enable with core clock and interrupts
}

// called at 880 Hz
void SysTick_Handler(void)
{
	unsigned int currSW, preSW, count; //defining and initialising variables
	
    currSW = GPIO_PORTA_DATA_R&0x08; // assigning PA3 as input switch
    if ((preSW == 0x00) && (currSW == 0x08)) // checking if input switch pressed
    {
        count++;
    }
    if ((count%2) == 0) 
    {
        GPIO_PORTA_DATA_R &= ~0x04; // turn off output sound on PA2
    }
    else
    {
        GPIO_PORTA_DATA_R ^= 0x04; // toggle output to generate sound
    }
    preSW = currSW;
}

// Port A initialization as Switch PA3

void Switch_Init(void)
{
    unsigned long volatile delay;
    SYSCTL_RCGC2_R |= 0x00000001; // (a) activate clock for port A
    delay = SYSCTL_RCGC2_R;
    GPIO_PORTA_DIR_R &= ~0x08;                             // (c) make PA3 input
    GPIO_PORTA_AFSEL_R &= ~0x08;                           //     disable alt funct on PA3
    GPIO_PORTA_DEN_R |= 0x08;                              //     enable digital I/O on PA3
    GPIO_PORTA_PCTL_R &= ~0x0000F000;                      //  configure PA3 as GPIO
    GPIO_PORTA_AMSEL_R &= ~0x08;                           //     disable analog functionality on PA3
    GPIO_PORTA_PDR_R |= 0x08;                              //     enable weak pull down on PA3
    GPIO_PORTA_IS_R &= ~0x08;                              // (d) PA3 is edge-sensitive
    GPIO_PORTA_IBE_R &= ~0x08;                             //     PA3 is not both edges
    GPIO_PORTA_IEV_R |= 0x08;                              //     PA3 rising edge event
    GPIO_PORTA_ICR_R = 0x08;                               // (e) clear flag 3
    GPIO_PORTA_IM_R |= 0x08;                               // (f) arm interrupt on PA3
    NVIC_PRI7_R = (NVIC_PRI7_R & 0xFF00FFFF) | 0x00400000; // (g) priority 2
    NVIC_EN0_R = 0x40000000;                               // (h) (interrupt no. 30 of NVIC selected)
}

int main(void)
{ 
	
		PLL_Init(); // calling port F initialization
    Sound_Init(); // calling sound initialization dunction
    Switch_Init(); // calling switch initialization function
    while (1)
    {
			// Systick interrupt calls the interrupt handler to do functions
			//main program free

    }
}
void SystemInit() {}; // System specific function executed before main

