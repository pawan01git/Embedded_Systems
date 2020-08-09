// ***** 0. Documentation Section *****
// main.c for Functional Debugging
// Runs on TM4C123
// In this lab we are learning functional debugging by dumping
//   recorded I/O data into an array
// January 30, 2020

// Pawan K

// ***** 1. Pre-processor Directives Section *****

#include "tm4c123gh6pm.h" // calling file that contains port addresses 

#define SWR       (*((volatile unsigned long *)0x4002504C)) // register address for SW1,SW2, PF1 (Red LED)



/* 
This Lab9 starter project is the same as C9_Debugging example but 
includes the connections to the Lab9 grader. You will make three changes. 
First, make the LED flash at 10 Hz. In other words, make it turn on for 0.05 seconds, 
and then turn off for 0.05 seconds. 
Second, make the LED flash if either switch SW1 or SW2 is pressed 
(this means flash the LED if either PF4 or PF0 is 0). 
Third, record PortF bits 4,1,0 every time the input changes or the output changes. 
For example, if your system detects a change in either PF4 or PF0 input, 
record PortF bits 4,1,0. If your system causes a change in PF1, record PortF bits 4,1,0. 

If both PF4 and PF0 switch are not pressed, the PF1 output should be low.  
If either PF4 or PF0 switches is pressed, the output toggles at 10 Hz (±10%). 
Information collected in the Data array matches the I/O on PortF.
50 data points are collected only on a change in input or a change in output.
This means no adjacent elements in the array should be equal.

Switches SW1 and SW2 are connected to Port F pins PF4 and PF0 respectively.
When either SW1 or SW2 will be pressed LED will toggle will frequency 1000 Hz. 
In other cases LED would be off.  The frequencs is controlled by Systick so
it is very accurate withing range ±10%. To verify it data from SysTick current register is getting dumped
in array when there is any change in PF4,PF1 or PF0. And later the frequency would be logically compared from
difference in value of SysTick Current register betweenn adjacent changes.
*/
void PortF_Init (void); // PortF initialization function prototype
void Delay(void); // Delay function prototype
void SysTick_Init(void); // SysTick function prototype


int main(void){
unsigned long  Time[50],Data[50], Curr[50];  // Array to store data from StysTick current, PF4,PF1,PF0
	unsigned long i,last,now,diff,SW;
  PortF_Init();   // PortF initialization function
  SysTick_Init(); // Systick initialization function
  i = 0;          // array index
  last = NVIC_ST_CURRENT_R; // storing value from SysTick current register
  while(1){
		now = SWR;
     diff = last - now;	
		last = now;
		SW = SWR&0x11;
		if (SW == 0x11) // when no switch pressed
		{
			SWR &= ~0x02;  // LED off
		}
		else
		{
			 SWR ^= 0x02;    // toggle red LED
		}         
		
    if (i<50 && (diff != 0)){
      now = NVIC_ST_CURRENT_R;
			Curr[i] = NVIC_ST_CURRENT_R;
      Time[i] = (last-now)&0x00FFFFFF;  // 24-bit time difference
      Data[i] = SWR; // record PF1
      last = now;
      i++;
    }
    Delay();
  }
}
void PortF_Init(void){ volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000020;     // 1) activate clock for Port F
  delay = SYSCTL_RCGC2_R;           // allow time for clock to start
  GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock GPIO Port F
  GPIO_PORTF_CR_R |= 0x01;           // allow changes to PF4-0
  // only PF0 needs to be unlocked, other bits can't be locked
  GPIO_PORTF_AMSEL_R = 0x00;        // 3) disable analog on PF
  GPIO_PORTF_PCTL_R = 0x00000000;   // 4) PCTL GPIO on PF4-0
  GPIO_PORTF_DIR_R |= 0x02;          // 5) PF1 output
  GPIO_PORTF_AFSEL_R &= ~0x1F;        // 6) disable alternate function on PF4-0
  GPIO_PORTF_PUR_R |= 0x11;          // enable pull-up on PF0 and PF4
  GPIO_PORTF_DEN_R = 0x1F;          // enable digital I/O on PF4-0
}

// Initialize SysTick with busy wait running at bus clock.
void SysTick_Init(void){
  NVIC_ST_CTRL_R = 0;                   // disable SysTick during setup
  NVIC_ST_RELOAD_R = 0x00FFFFFF;        // maximum reload value
  NVIC_ST_CURRENT_R = 0;                // any write to current clears it             
  NVIC_ST_CTRL_R = 0x00000005;          // enable SysTick with core clock enable, interrupt and counter flags
}

void Delay(void){unsigned long volatile time;
  time = 80000; // 0.1sec
  while(time){
   time--;
  }
}

void SystemInit(void) {}; // compiler specifinc function, executes before main

// Color    LED(s) PortF
// dark     ---    0
// red      R--    0x02
// blue     --B    0x04
// green    -G-    0x08
