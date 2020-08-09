// DAC.h
// Runs on TM4C123, 
// Header file for the 4-bit digital to analog converter
// Pawan K
// 24 March 2020

// **************DAC_Init*********************
// Initialize 4-bit DAC 
// Input: none
// Output: none
void DAC_Init(void);


// **************DAC_Out*********************
// output to DAC
// Input: 4-bit data, 0 to 15 
// Output: none
void DAC_Out(unsigned long data);
  


