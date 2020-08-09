// Piano.h
// Runs on TM4C123, 
// There are four keys in the piano
// Pawan K
// 23 March 2020

// **************Piano_Init*********************
// Initialize piano key inputs
// Input: none
// Output: none
void Piano_Init(void);


// **************Piano_In*********************
// Input from piano key inputs
// Input: none 
// Output: 0 to 15 depending on keys
// 0x01 is key 0 pressed, 0x02 is key 1 pressed,
// 0x04 is key 2 pressed, 0x08 is key 3 pressed
unsigned long Piano_In(void);
