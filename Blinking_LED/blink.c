// This program runs on to run on the Texa LaunchPad Tiva4c123gxl
// If just SW1 switch is pressed,            the RGB should be red
// If just SW2 switch is pressed,            the RGB should be blue
// If both switches SW1 and SW2 are pressed, the RGB should be green
// If neither SW1 or SW2 is pressed,         the RGB should be off

// Documentation Section
// main.c
// Inputs from PF4,PF0, output to PF3,PF2,PF1 (Green, Blue, Red respectively)
// Author: Pawan K
// Date: Jan 5, 2019
// LaunchPad built-in hardware
// SW1 left switch is negative logic PF4 on the Launchpad
// SW2 right switch is negative logic PF0 on the Launchpad
// Pre-processor Directives Section
// Constant declarations to access port registers using
// symbolic names instead of addresses
// TExaS.o is the object code for the real-board grader
//#include "tm4c123gh6pm.h"

//Register Addressing

#define SYSCTL_RCGC2_R (*((volatile unsigned long *)0x400FE108))     // offset address of Systick Port F
#define GPIO_PORTF_LOCK_R (*((volatile unsigned long *)0x40025520))  // offset address to unlock Port F special pins
#define GPIO_PORTF_CR_R (*((volatile unsigned long *)0x40025524))    //offset address to enable commit on Port F
#define GPIO_PORTF_PCTL_R (*((volatile unsigned long *)0x4002552C))  // address of Port control register on Port F
#define GPIO_PORTF_DATA_R (*((volatile unsigned long *)0x400253FC))  // Port F data register address
#define LED (*((volatile unsigned long *)0x40025038))                // Register address of 3 LEDs
#define GPIO_PORTF_AFSEL_R (*((volatile unsigned long *)0x40025420)) // offset address of alternate function register on Port F
#define GPIO_PORTF_AMSEL_R (*((volatile unsigned long *)0x40025528)) // register to control analog functionon Port F
#define GPIO_PORTF_PUR_R (*((volatile unsigned long *)0x40025510))   // pull down control registeron Port F
#define GPIO_PORTF_DIR_R (*((volatile unsigned long *)0x40025400))   // data direction control register Port F
#define GPIO_PORTF_DEN_R (*((volatile unsigned long *)0x4002551C))   //digital enable control register on Port F

//Port Initialization

void PortF_Init(void)
{
    volatile unsigned long delay;   // defining and initializing a variable
    SYSCTL_RCGC2_R |= 0x00000020;   // Enabling system clock for Port F
    delay = SYSCTL_RCGC2_R;         // doing random process to spend 3 cycles for delay and let clock stabilise
    GPIO_PORTF_LOCK_R = 0x4C4F434B; // Writing standard value to unlock Port F for commit
    GPIO_PORTF_CR_R = 0x01;         // enabling PF0 for commit to enable pull  down as special pin on pg. 1329
    GPIO_PORTF_AFSEL_R = 0x00;      //   no alternate function
    GPIO_PORTF_PCTL_R &= ~0x0F;     // reseting the PF0, nibble addressing
    GPIO_PORTF_AMSEL_R = 0x00;      // disable analog function
    GPIO_PORTF_DEN_R |= 0x1F;       // enabling digital function on Pin PF0-PF4
    GPIO_PORTF_DIR_R |= 0x0E;       //making PF1-PF3 output
    GPIO_PORTF_PUR_R = 0x11;        // enable pull down register for SW1,SW2
}

// main program starts here

void PortF_Init(void); // PortF initialization prototype
void Delay(int);       // Dalay function prototype

int main(void)
{
    volatile unsigned long SW1, SW2;
    PortF_Init(); // calling Port F initialization function
    while (1)
    {
        SW1 = GPIO_PORTF_DATA_R & 0x10;
        SW2 = GPIO_PORTF_DATA_R & 0x01;

        if ((SW1 == 0x10) && (SW2 == 0x01)) // no switches pressed
        {
            LED = 0x00; // turning off LED
            Delay(100);  // 100 ms delay
        }
        else if ((SW1 == 0x00) && (SW2 == 0x01)) // Only SW1 pressed
        {
            LED  ^= 0x02; // Red LED
            Delay(100); // 100 ms delay
        }
        else if ((SW1 == 0x10) && (SW2 == 0x00))// Only SW2 pressed
        {
            LED ^= 0x04; // Blue LED
            Delay(100); // 100 ms delay
        }

        else if ((SW1 == 0x00) && (SW2 == 0x00)) // Both SW1 and SW2 pressed
        {
            LED ^= 0x08; // Green LED
            Delay(100); // 100 ms delay
        }
    }
}

void Delay(int time)
{
    while (time > 0)
    {
        int i = 133333; // frequency 16 MHz, 3 clock cycles while loop, appx .83 ms
        while (i > 0)
        {
            i = i - 1;
        }
        time = time - 1;
    }
}

void SystemInit(void){}; // A function that can execute before main

