// MeasurementOfDistance.c
// Runs on TM4C123
//Runs at 80 Mhz
// In this project a variable resistance slider potentiometer-
// has been used. That translates the change in resitance as distance.
// Change is resistance is sampled and coverted in digital  format using-
// Analog to digital convertor ADC.
// Use SysTick interrupts to periodically initiate a software-
// triggered ADC conversion, convert the sample to a fixed-
// point decimal distance, and store the result in a mailbox.
// The foreground thread takes the result from the mailbox,
// converts the result to a string, and prints it to the
// Nokia5110 LCD.  
// 10 April 2020



// Slide pot pin 3 connected to +3.3V
// Slide pot pin 2 connected to PE2(Ain1) and PD3
// Slide pot pin 1 connected to ground

#include "PLL.h"
#include "ADC.h"
#include "tm4c123gh6pm.h"
#include "Nokia5110.h"

unsigned char String[10]; // null-terminated ASCII string
int Distance;   // units 0.001 cm
unsigned long ADCdata;    // 12-bit 0 to 4095 sample
unsigned long Flag;       // 1 means valid Distance, 0 means Distance is empty

// Port F, PF1 as digital output initialization
void PortF_Init(void){ volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000020;     // PortF clock
  delay = SYSCTL_RCGC2_R;           // delay    
	GPIO_PORTF_PCTL_R &= ~0x02; 	 // prot control
  GPIO_PORTF_AMSEL_R |= 0x02;        //disable analog function
	 GPIO_PORTF_DEN_R |= 0x02;  // enable digital function
  GPIO_PORTF_DIR_R |= 0x02;          // PF1,PF6,PF7 output, others remain input  
  GPIO_PORTF_AFSEL_R &= ~0x02;        // no alternate function
  GPIO_PORTF_PDR_R |= 0x02;          // enable pull down register    	 
}

//********Convert****************
// Convert a 12-bit binary ADC sample into a 32-bit unsigned
// fixed-point distance (resolution 0.001 cm).  Calibration
// data is gathered using known distances and reading the
// ADC value measured on PE1.  
// Overflow and dropout should be considered 
// Input: sample  12-bit ADC sample
// Output: 32-bit distance (resolution 0.001cm)
int Convert(unsigned long sample){
	Distance = (0.1465*sample); // conversion factor from calibration
	return Distance;
}

// Initialize SysTick interrupts to trigger at 40 Hz, 25 ms
void SysTick_Init(unsigned long period){
  NVIC_ST_CTRL_R = 0;         // disable SysTick during setup
  NVIC_ST_RELOAD_R = period -1;// reload value for frequency
  NVIC_ST_CURRENT_R = 0;      // any write to current clears it
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x20000000; // priority 1      
  NVIC_ST_CTRL_R = 0x0007;  // enable SysTick with core clock and interrupts
  
}
// executes every 25 ms, collects a sample, converts and stores in mailbox
void SysTick_Handler(void){
	GPIO_PORTF_DATA_R ^= 0x02; // toggle PF1 red LED
	GPIO_PORTF_DATA_R ^= 0x02; // toggle PF1 red LED
	ADCdata = ADC0_In();
	Distance = Convert(ADCdata);
	Flag = 1;
	
}

//-----------------------UART_ConvertDistance-----------------------
// Converts a 32-bit distance into an ASCII string
// Input: 32-bit number to be converted (resolution 0.001cm)
// Output: store the conversion in global variable String[10]
// Fixed format 1 digit, point, 3 digits, space, units, null termination
// Examples
//    4 to "0.004 cm"  
//   31 to "0.031 cm" 
//  102 to "0.102 cm" 
// 2210 to "2.210 cm"
//10000 to "*.*** cm"  any value larger than 9999 converted to "*.*** cm"
void UART_ConvertDistance(unsigned long n){
// as part of Lab 11 you implemented this function
	// as part of Lab 11 implement this function
        if (n > 6000)
        {
            String[0] = '*';
            String[1] = '.';
            String[2] = '*';
            String[3] = '*';
            String[4] = '*';
            String[5] = ' ';
            String[6] = 'c';
            String[7] = 'm';
            String[8] = '\0';
        }
        else
        {
            String[0] = ((n / 1000) % 10) + 0x30;
            String[1] = '.';
            String[2] = ((n / 100) % 10) + 0x30;
            String[3] = ((n / 10 % 10)) + 0x30;
            String[4] = (n % 10) + 0x30;
            String[5] = ' ';
            String[6] = 'c';
            String[7] = 'm';
            String[8] = '\0';
        }
 
}


// once the ADC is operational, you can use main2 to debug the convert to distance
int main(void){ 
	PLL_Init();
  ADC0_Init();    // initialize ADC0, channel 1, sequencer 3
  Nokia5110_Init();             // initialize Nokia5110 LCD
	SysTick_Init(2000000); // sampling at 40 Hz
		PortF_Init();	 // PortF initialization
	Nokia5110_SetCursor(0,5);
	Nokia5110_OutString("D = ");

  while(1){ 
		Flag = 0;
		while (Flag == 0) {};		
			Nokia5110_SetCursor(4,5);
   UART_ConvertDistance(Distance); // UART output for display
   Nokia5110_OutString(String);    // output to Nokia5110 LCD (optional)
  }
}
void SystemInit(void) {}; // system specific function executes before main

