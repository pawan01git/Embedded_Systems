// Pacemaker, pacemaker.c
// Runs on TM4C123 LaunchPad
// Input from PF4(SW1) is AS (atrial sensor), 
// Output to PF3, Green LED, is Ready,
// Output to PF1, Red LED, is VT (ventricular trigger) 
// Make PF4 input, PF3,PF1 output
// Initialize Ready to high and VT to low
// Repeat this sequence of operation over and over
// 1) Wait for AS to fall (touch SW1 switch)
// 2) Clear Ready low
// 3) Wait 10ms (debounces the switch)
// 4) Wait for AS to rise (release SW1)
// 5) Wait 250ms (simulates the time between atrial and ventricular contraction)
// 6) set VT high, which will pulse the ventricles 
// 7) Wait 250ms
// 8) clear VT low 
// 9) set Ready high

// Date: 20 January 2020

// 1. Pre-processor Directives Section
// Memory mapped register addressing
#define GPIO_PORTF_DATA_R       (*((volatile unsigned long *)0x400253FC)) // register address of Port F
#define SW1       							(*((volatile unsigned long *)0x40025040)) // register addressing of SW1
#define Green       						(*((volatile unsigned long *)0x40025020)) // register address of Green
#define Red       							(*((volatile unsigned long *)0x40025008)) // register address of Red
#define GPIO_PORTF_DIR_R        (*((volatile unsigned long *)0x40025400)) // register address of Port F direction register
#define GPIO_PORTF_AFSEL_R      (*((volatile unsigned long *)0x40025420))// register address of Port F alternate function register
#define GPIO_PORTF_PUR_R        (*((volatile unsigned long *)0x40025510))// register address of Port F pull up 
#define GPIO_PORTF_DEN_R        (*((volatile unsigned long *)0x4002551C))// register address of Port F  digital enable
#define GPIO_PORTF_LOCK_R       (*((volatile unsigned long *)0x40025520)) // Unlock register of Port F
#define GPIO_PORTF_CR_R         (*((volatile unsigned long *)0x40025524)) // address of Port F commit register
#define GPIO_PORTF_AMSEL_R      (*((volatile unsigned long *)0x40025528))// address of Port F analog function
#define GPIO_PORTF_PCTL_R       (*((volatile unsigned long *)0x4002552C)) // address of Port F port control
#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))// address of Port F systick clock
	
// 2. Declarations Section
//   Function Prototypes

void PortF_Init(void); // prototype of Port F initializing function
void SetVT(void);// prototype of setVT function
void ClearVT(void);//  prototype of clearVT function
void SetReady(void);//  prototype of setReady function
void ClearReady(void);//  prototype of clearReady function
void Delay(unsigned long msec);//  prototype of Delay function
void WaitForASLow(void);//  prototype of ASLow function
void WaitForASHigh(void);//  prototype of ASHigh function

// main function
int main(void){ 
  PortF_Init();    // calling Port F initializing function
	while (1)
	{	
ClearVT();       // VT signal goes low
SetReady();      // Ready signal goes high
WaitForASLow();	 // wait for switch to be pressed
ClearReady();	 // Ready signal goes low
Delay(10);	 // wait 10ms
WaitForASHigh(); // wait for switch to be released
Delay(250);	 // wait 250ms
SetVT();	 // VT signal goes high
Delay(250);	 // wait 250ms

	}
		}


void PortF_Init(void){ volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000020;      // 1) F clock
  delay = SYSCTL_RCGC2_R;            // delay to allow clock to stabilize     
  GPIO_PORTF_AMSEL_R &= 0x00;        // 2) disable analog function
  GPIO_PORTF_PCTL_R &= 0x00000000;   // 3) GPIO clear bit PCTL  
  GPIO_PORTF_DIR_R &= ~0x10;         // 4.1) PF4 input,
  GPIO_PORTF_DIR_R |= 0x0E;          // 4.2) PF3,2,1 output  
  GPIO_PORTF_AFSEL_R &= 0x00;        // 5) no alternate function
  GPIO_PORTF_PUR_R |= 0x10;          // 6) enable pullup resistor on PF4       
  GPIO_PORTF_DEN_R |= 0x1E;          // 7) enable digital pins PF4-PF1
}

void SetVT(void){
Red = 0x02; // turning Red LED on
}

void ClearVT(void){
Red = 0x00; // turning Red LED off
}

void SetReady(void){
Green = 0x08; // turning Green LED on
	
}
void ClearReady(void){
Green = 0x00; // turning Green LED off

}


void WaitForASLow(void)
{
while(SW1 == 0x10)
{}
}

void WaitForASHigh(void)
{
while(SW1 == 0x00) // delay when SW1 pressed
{}
}

	void Delay(unsigned long time){
  unsigned long i;
  while(time > 0){
    i = 13333;  // to create 1ms delay
    while(i > 0){
      i = i - 1;
    }
    time = time - 1; // decrements after 1ms
  }
}
	void SystemInit(void) // Board specific function implement before main
	{};

