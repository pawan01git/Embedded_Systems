// UART.c
// Runs on TM4C123
//Using UART0
// Pawan K
// 10 Feb 2020
// this connection occurs in the USB debugging cable
// U0Rx (PA0) connected to serial port on PC
// U0Tx (PA1) connected to serial port on PC
// Ground connected ground in the USB cable

#include "tm4c123gh6pm.h" // header library for register address and functions
#include "UART.h" // library for UART functions

//------------UART_Init------------
// Initialize the UART for 115200 baud rate (assuming 80 MHz UART clock),
// 8 bit word length, no parity bits, one stop bit, FIFOs enabled
// Input: none
// Output: none
void UART_Init(void)
{

    SYSCTL_RCGC1_R |= SYSCTL_RCGC1_UART0; // activate system clock for UART0
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOA; // activate port A
    UART0_CTL_R &= ~UART_CTL_UARTEN;      //  UARTCTL clear
    UART0_IBRD_R = 43;                    // IBRD = int(80,000,000 / (16 * 115200)) = int(43.402778)
    UART0_FBRD_R = 26;                    // FBRD = fractional part of IBRD *64 + 0.5= round(0.402778 * 64) = 26
                                          
    UART0_LCRH_R = (UART_LCRH_WLEN_8 | UART_LCRH_FEN);// 8 bit word length (no parity bits, one stop bit, FIFOs)
    UART0_CTL_R |= UART_CTL_UARTEN; // enable UART
    GPIO_PORTA_AFSEL_R |= 0x03;     // enable alt funct on PA1,PA0
    GPIO_PORTA_DEN_R |= 0x03;       // enable digital I/O on PA1,PA0
                                    // configure PA1,PA0 as UART0
    GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R & 0xFFFFFF00) + 0x00000011; // port control
    GPIO_PORTA_AMSEL_R &= ~0x03; // disable analog functionality on PA1,PA0
}

//------------UART_InChar------------
// Wait for new serial port input
// Input: none
// Output: ASCII code for key typed
unsigned char UART_InChar(void)
{
    while ((UART0_FR_R & UART_FR_RXFE) != 0);   
    return ((unsigned char)(UART0_DR_R & 0xFF));
}

//------------UART_InCharNonBlocking------------
// Get oldest serial port input and return immediately
// if there is no data.
// Input: none
// Output: ASCII code for key typed or 0 if no character
unsigned char UART_InCharNonBlocking(void)
{
    if ((UART0_FR_R & UART_FR_RXFE) == 0) // Rx FIFO not empty
    {
        return ((unsigned char)(UART0_DR_R & 0xFF));
    }
    else
    {
        return 0;
    }
}

//------------UART_OutChar------------
// Output 8-bit to serial port
// Input: letter is an 8-bit ASCII character to be transferred
// Output: none
void UART_OutChar(unsigned char data)
{
    while ((UART0_FR_R & UART_FR_TXFF) != 0); // Tx FIFO full
    UART0_DR_R = data;
}

//------------UART_InUDec------------
// InUDec accepts ASCII input in unsigned decimal format
//     and converts to a 32-bit unsigned number
//     valid range is 0 to 4294967295 (2^32-1)
// Input: none
// Output: 32-bit unsigned number
// If you enter a number above 4294967295, it will return an incorrect value
// Backspace will remove last digit typed
unsigned long UART_InUDec(void)
{
    unsigned long number = 0, length = 0;
    char character;
    character = UART_InChar();
    while (character != CR)
    { // accepts until <enter> is typed
        // The next line checks that the input is a digit, 0-9.
        // If the character is not 0-9, it is ignored and not echoed
        if ((character >= '0') && (character <= '9'))
        {
            number = 10 * number + (character - '0'); // this line overflows if above 4294967295
            length++;
            UART_OutChar(character);
        }
        // If the input is a backspace, then the return number is
        // changed and a backspace is outputted to the screen
        else if ((character == BS) && length)
        {
            number /= 10;
            length--;
            UART_OutChar(character);
        }
        character = UART_InChar();
    }
    return number;
}
//------------UART_OutString------------
// Output String (NULL termination)
// Input: pointer to a NULL-terminated string to be transferred
// Output: none
void UART_OutString(unsigned char String[])
{
    int i = 0;
    while (String[i])
    {
        UART_OutChar(String[i++]);
    }
}

unsigned char String[10];
//-----------------------UART_ConvertUDec-----------------------
// Converts a 32-bit number in unsigned decimal format
// Input: 32-bit number to be transferred
// Output: store the conversion in global variable String[10]
// Fixed format 4 digits, one space after, null termination
// Examples
//    4 to "   4 "
//   31 to "  31 "
//  102 to " 102 "
// 2210 to "2210 "
//10000 to "**** "  any value larger than 9999 converted to "**** "
void UART_ConvertUDec(unsigned long n)
{
    int a, i = 3;
    if (n >= 10000)
    {
        for (i = 3; i >= 0; i--)
        {
            String[i] = '*';
        }
        String[4] = ' ';
        String[5] = '\0';
    }
    else if (n > 0)
    {

        while (n > 0)
        {
            a = n % 10;
            String[i] = a + 0x30;
            n /= 10;
            i = i - 1;
        }
        while (i >= 0)
        {
            String[i] = ' ';
            i = i - 1;
        }
        String[4] = ' ';
        String[5] = '\0';
    }
    else
    {
        String[3] = 0 + 0x30;

        for (i = 2; i >= 0; i--)
        {
            String[i] = ' ';
        }
        String[4] = ' ';
        String[5] = '\0';
    }
	}

//-----------------------UART_OutUDec-----------------------
// Output a 32-bit number in unsigned decimal format
// Input: 32-bit number to be transferred
// Output: none
// Fixed format 4 digits, one space after, null termination
void UART_OutUDec(unsigned long n)
{
        UART_ConvertUDec(n);    // convert using your function
        UART_OutString(String); // output using your function
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
    void UART_ConvertDistance(unsigned long n)
    {
        if (n >= 10000)
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

    //-----------------------UART_OutDistance-----------------------
    // Output a 32-bit number in unsigned decimal fixed-point format
    // Input: 32-bit number to be transferred (resolution 0.001cm)
    // Output: none
    // Fixed format 1 digit, point, 3 digits, space, units, null termination
    void UART_OutDistance(unsigned long n)
    {
        UART_ConvertDistance(n); // convert using your function
        UART_OutString(String);  // output using your function
    }
