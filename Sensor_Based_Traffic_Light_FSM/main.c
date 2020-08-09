// ***** 0. Documentation Section *****
// main.c for Traffic_Light_FSM
// Runs on TM4C123
// Index implementation of a Moore finite state machine to operate a traffic light.  
// Pawan K
// 5 February 2020
// 80 MHz PLL clock used
// PB0 -PB5 for West and South (This will be used for output)
//PE0 - PE3 for input pedestrian, car or N/S, car on E/W respectively
// 2 sec in go state, 1 sec in wait state, 0.5 sec in blink
//PF1 and PF3 for red and green light for pedestrian respectively
// east/west red light connected to PB5
// east/west yellow light connected to PB4
// east/west green light connected to PB3
// north/south facing red light connected to PB2
// north/south facing yellow light connected to PB1
// north/south facing green light connected to PB0
// "walk" light connected to PF3 (built-in green LED)
// "don't walk" light connected to PF1 (built-in red LED)


// *****  Library  Section *****
#include "tm4c123gh6pm.h" // library containing register address
#include "SysTick.h" // library for SysTick registers and functions
#include "PLL.h" // PLL Library


// ***** Pre-processor Directives Section *****


#define T500ms 10 // preprocessor directives for 500 ms
#define T2sec 100 // preprocessor directives for 2 sec
#define T1sec 50 // preprocessor directives for 1 sec
// ***** 2. Global Declarations Section *****

// FUNCTION PROTOTYPES: Each subroutine defined

void PortB_Init(void); // Port B initializer prototype
void PortE_Init(void); // Port E initializer prototype
void PortF_Init(void); // Port F initializer prototype

unsigned long TLight_Input(void); // prototype for function to read inputs
void TLight_Output1(unsigned long output1); // prototype for PortF output function
void TLight_Output2(unsigned long output2); // prototype for PortB output function
 
// Defining structure for Moore state

struct State {
unsigned long out1; //output
unsigned long out2; //output
unsigned long delay; // time delay for states
unsigned long next[10];// next states
};

typedef const struct State StateType; // alias name for struct variable
//typedef StateType *StatePtr; // pointer, not sure for what

#define GS  0 //0x21,0x02
#define GW 1 // 0x0C,0x02
#define GP 2 //0x24,0x08
#define WS 3 //0x22,0x02
#define WW 4 //0x14,0x02
#define WP 5 //0x24,0x02
#define b1_on 6 //0x24,0x02
#define b1_of 7 //0x24,0x0
#define b2_on 8 //0x24,0x02
#define b2_of 9 //0x24,0x0

// defining output and input address
#define Output1 (*((volatile unsigned long *)0x400050FC)) 
#define Output2 (*((volatile unsigned long *)0x40025028)) 

 
StateType fsm[10] = {
{0x21,0x02,T2sec,{GS,WS,GS,WS,WS,WS,WS,WS}},//state goSouth
{0x0C,0x02,T2sec,{GW,GW,WW,WW,WW,WW,WW,WW}},//state goWest
{0x24,0x08,T2sec,{GP,b1_on,b1_on,b1_on,GP,b1_on,b1_on,b1_on}},//state goPedestrian
{0x22,0x02,T1sec,{GW,GW,GW,GW,GP,GW,GP,GW}},//state waitSouth
{0x14,0x02,T1sec,{GS,GS,GS,GS,GP,GP,GS,GP}},//sate waitWest
{0x24,0x02,T1sec,{GS,GW,GS,GW,GW,GW,GS,GS}},//state waitPedestrian
{0x24,0x02,T500ms,{b1_of,b1_of,b1_of,b1_of,b1_of,b1_of,b1_of,b1_of}},//state redBlinkOn
{0x24,0x00,T500ms,{b2_on,b2_on,b2_on,b2_on,b2_on,b2_on,b2_on,b2_on}},//state redBlinkOff
{0x24,0x02,T500ms,{b2_of,b2_of,b2_of,b2_of,b2_of,b2_of,b2_of,b2_of}},//state redBlinkOn
{0x24,0x00,T500ms,{WP,WP,WP,WP,WP,WP,WP,WP}} //state redBliinkOff
};
unsigned long S; // index to current state
unsigned long Input; 
//unsigned long Output1;
//unsigned long Output2;


// ***** 3. Subroutines Section *****

int main(void){
PortB_Init(); // calling PortB initialization
PortE_Init(); //calling PortE initialization
PortF_Init(); //calling PortF initialization
SysTick_Init();// calling Systick initialization
PLL_Init(); // PLL initialization
S = GS;  // Initial state go south

  while(1){
Output1 = fsm[S].out1; // set output for Port B
Output2 = fsm[S].out2;// set output for Port F
SysTick_Wait10ms(fsm[S].delay); // wait as defined in particular state
Input = TLight_Input(); // read from Port E input sensors
S = fsm[S].next[Input];// next state based on current state and input 
     
  }
}

void PortB_Init(void){
volatile unsigned long delay;
SYSCTL_RCGC2_R |= 0x00000002;     // activate system clock for Port B
delay = SYSCTL_RCGC2_R; // allow time for clock to stabilize
GPIO_PORTB_LOCK_R = 0x4C4F434B;   //  unlock GPIO Port B
GPIO_PORTB_CR_R |= 0x3F;           // allow changes to PB5-0
GPIO_PORTB_AMSEL_R &= ~0x3F;        //  disable analog on Port B
GPIO_PORTB_PCTL_R &= 0x00000;   //  PCTL GPIO on PB5-0
GPIO_PORTB_DIR_R |= 0x3F;          // PB5 - 0 as Output
GPIO_PORTB_AFSEL_R &= ~0x3F;        // 6) disable alternate function PB5-0
GPIO_PORTB_DEN_R |= 0x3F;          // 7) enable digital I/O on PB5-0

}

void PortE_Init(void){
volatile unsigned long delay;
SYSCTL_RCGC2_R |= 0x00000010;     // activate clock for Port E
delay = SYSCTL_RCGC2_R; // allow time for clockk to stabilize
//GPIO_PORTE_LOCK_R = 0x4C4F434B;   //  unlock GPIO Port E
//GPIO_PORTE_CR_R |= 0x07;           // allow changes to PE2-0
GPIO_PORTE_AMSEL_R &= ~0x07;        //  disable analog on Port E
GPIO_PORTE_PCTL_R &= 0x000;   //  making all pins GPIO
GPIO_PORTE_AFSEL_R &= ~0x07;        // 6) disable alternate function PE2-0
GPIO_PORTE_PDR_R &= ~0x07;        //enabling pull down resistor on input
GPIO_PORTE_DEN_R |= 0x07;          // 7) enable digital I/O on PE3-0
}

void PortF_Init(void){
volatile unsigned long delay;
SYSCTL_RCGC2_R |= 0x00000020;     //activate clock for Port F
delay = SYSCTL_RCGC2_R;          // allow time for clock to start
GPIO_PORTF_AMSEL_R &= ~0x0A;        //  disable analog on Port F PF3 and PF0
GPIO_PORTF_PCTL_R &= 0x0000;   //  making all pins GPIO
GPIO_PORTF_DIR_R |= 0x0A;          // PF3 and PF0
GPIO_PORTF_AFSEL_R &= ~0x0A;        // 6) disable alternate function PF3 and PF0
GPIO_PORTF_DEN_R |= 0x0A;          // 7) enable digital I/O on PF3 and PF0
}



// **************Sensor Input*****************
//000 None
// 001 West
// 010 South
// 011 South West
//100 Pedestrian
// 101 Pedestrian West
// 110 Pedestrian South
// 111 Pedestrian South West
// but we will serve in South West Pedestrian order
#define INPUT (*((volatile unsigned long *)0x4002401C)) // defining address of i/p pins
unsigned long TLight_Input(void){
return ((INPUT&0x01) + (INPUT&0x02) + (INPUT&0x04)); //combine the inputs from sensor
// into Decimal system to match the Array Index

}

void SystemInit(void) {}; // System specific function that executes before main
