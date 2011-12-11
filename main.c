/*

	LCD/Joystick Demo
	
	This demo uses a 9 pin Apple II compatable joystick and displays its output on a
	Hitachi HD44780 compatible LCD.

	Device: PIC24FJ64GB004
	
	Pin out:
	LCD:
		RS:	RB11
		E: 	RB10
		D4:	RB9
		D5:	RB8
		D6:	RB7
		D7:	RB5
	
	NOTE: 	These pins were chosen because they are 5V tolerant. Using other pins with a 5V display
			may cause magic smoke.
	 
	Joystick:
		X-Axis:		RA0
		Y-Axis: 	RA1
		Button 1:	RB14
		Button 2:	RB15

	NOTE:	1) Power the joystick with 3.3 V, otherwise you will toast the microcontroller.
			2) X and Y axis pins must be connected to ground through 10 kOhm resistor
			(on these joysticks, the potentiometer is not internally connected to ground,
			adding this resistor creates a divider between the joystick resistance and
			ground)

	For Apple II joysticks, you can find the pinout here: http://www.1000bit.it/support/manuali/apple/R023PINOUTS.TXT
	as the "Apple Game Port (9-pin connector)"

	Copyright (c) 2011 Eric Evenchick (www.evenchick.com)

	Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
	and associated documentation files (the "Software"), to deal in the Software without restriction, 
	including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
	and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
	subject to the following conditions:
	
	The above copyright notice and this permission notice shall be included in all copies or
	substantial portions of the Software.
	
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT 
	LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO 
	EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
	WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
	SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#include <p24fxxxx.h>

_CONFIG1( JTAGEN_OFF & GCP_OFF & GWRP_OFF & ICS_PGx1 & FWDTEN_OFF)
_CONFIG2( FNOSC_PRIPLL & FCKSM_CSDCMD & OSCIOFNC_OFF)

// LCD pins
#define RS 		LATBbits.LATB11
#define RSTRIS 	TRISBbits.TRISB11
#define E 		LATBbits.LATB10
#define ETRIS 	TRISBbits.TRISB10
#define D4 		LATBbits.LATB9
#define D4TRIS 	TRISBbits.TRISB9
#define D5 		LATBbits.LATB8
#define D5TRIS	TRISBbits.TRISB8
#define D6 		LATBbits.LATB7
#define D6TRIS 	TRISBbits.TRISB7
#define D7 		LATBbits.LATB5
#define D7TRIS 	TRISBbits.TRISB5

// button pins
#define BUTTON1 	PORTBbits.RB14
#define BUTTON1TRIS	TRISBbits.TRISB14
#define BUTTON2		PORTBbits.RB15
#define BUTTON2TRIS TRISBbits.TRISB15

// LCD Functions
int initLcd();
int sendCmd(int cmd);
int sendData(int data);
int puts(char *str);
int cls();

// ADC Initialization Function
int initAD();

int main()
{
	char buf[20] = ""; 
	long i;
	int digit;
	
	// store the most recent x and y values
	int x, y;
	// store the last x and y values
	int lastX, lastY;
	// store the last button presses
	char lastB1, lastB2;

	// set all LCD pins as outputs
	RSTRIS = 0;
	ETRIS = 0;
	D4TRIS = 0;
	D5TRIS = 0;
	D6TRIS = 0;
	D7TRIS = 0;

	// start with all inputs as digital
	AD1PCFG = 0xFFFF;
	// set button pins as inputs
	BUTTON1TRIS = 1;
	BUTTON2TRIS = 1;

	initLcd();
	puts("initializing...");
	initAD();
	
	for (;;)
	{
		// get AN0 (x axis)
		AD1CHS = 0;
		// start a conversion
		AD1CON1bits.SAMP = 1;
 		for (i=0; i<10; i++);
		AD1CON1bits.SAMP = 0;
		// wait until sample complete
		while(!AD1CON1bits.DONE);

		x = ADC1BUF0;

		// get AN1 (y axis)
		AD1CHS = 1;
		// start a conversion
		AD1CON1bits.SAMP = 1;
 		for (i=0; i<10; i++);
		AD1CON1bits.SAMP = 0;
		// wait until sample complete
		while(!AD1CON1bits.DONE);

		y = ADC1BUF0;

		// check if the values have changed
		if (lastX/10 == x/10 && lastY/10 == y/10 && BUTTON1 == lastB1 && BUTTON2 == lastB2)
		{
			// if not, skip this write
			continue;
		}

		// otherwise, do an LCD write

		// store the values of x, y and the buttons
		lastX = x;
		lastY = y;
		lastB1 = BUTTON1;
		lastB2 = BUTTON2;

		// build the LCD string 
		buf[0] = 'x';
		buf[1] = ':';
		buf[2] = ' ';
		digit = x / 1000;
		x = x - digit * 1000;
		buf[3] = digit + '0';
		digit = x / 100;
		x = x - digit * 100;	
		buf[4] = digit + '0';
		digit = x / 10;
		x = x - digit * 10;  
		buf[5] =  digit + '0'; 
		buf[6] =  x + '0';
		
		buf[7] = '\n';
		
		buf[8] = 'y';
		buf[9] = ':';
		buf[10] = ' ';

		digit = y / 1000;
		y = y - digit * 1000;
		buf[11] = digit + '0';
		digit = y / 100;
		y = y - digit * 100;	
		buf[12] = digit + '0';
		digit = y / 10;
		y = y - digit * 10;  
		buf[13] =  digit + '0'; 
		buf[14] =  y + '0';
		
		buf[15] = ' ';

		// check for button presses
		if (BUTTON1)
			buf[16] = 'A';
		else
			buf[16] = ' ';

		if (BUTTON2)
			buf[17] = 'B';
		else
			buf[17] = ' ';
		
		// null terminate
		buf[18] = '\0';
		
		// clear LCD and write string
		cls();
		puts(buf);
		// slow down the loop
		for(i=0; i < 20000; i++);
	}
}

int initLcd()
{
	long i;
	int j;
	RS = 0;
	// send 0x3 3 times
	for (j = 0; j < 3; j++)
	{
		D4 = 1;
		D5 = 1;
		D6 = 0;
		D7 = 0;
		E = 1;
		for (i = 0; i < 1000; i++);
		E = 0;
	}
	// enable 4 bit mode
	D4 = 0;
	D5 = 1;
	D6 = 0;
	D7 = 0;
	E = 1;
	for (i = 0; i < 1000; i++);
	E = 0;
	for (i = 0; i < 1000; i++);
	sendCmd(0x28);	// set interface
	sendCmd(0x08);	// turn off LCD
	sendCmd(0x01);	// clear lcd, return home
	sendCmd(0x06);	// set cursor direction to right
	sendCmd(0x0C);	// turn on display, no cursor
	return 0;
}

int sendCmd(int cmd)
{
	long i;

	// use command mode (RS=0)
	RS = 0;
	for (i = 0; i < 500; i++);

	// upper nibble
	D4 = cmd >> 4 & 0x1;
	D5 = cmd >> 5 & 0x1;
	D6 = cmd >> 6 & 0x1;
	D7 = cmd >> 7 & 0x1;
	// send and wait
	E = 1;
	for (i = 0; i < 100; i++);
	E = 0;
	for (i = 0; i < 100; i++);

	// lower nibble
	D4 = cmd & 0x1;
	D5 = cmd >> 1 & 0x1;
	D6 = cmd >> 2 & 0x1;
	D7 = cmd >> 3 & 0x1;
	// send and wait
	E = 1;
	for (i = 0; i < 100; i++);
	E = 0;
	for (i = 0; i < 100; i++);	

	return 0;
}

int sendData(int data)
{
	long i;

	// use data mode (RS=1)
	RS = 1;
	for (i = 0; i < 500; i++);

	// upper nibble
	D4 = data >> 4 & 0x1;
	D5 = data >> 5 & 0x1;
	D6 = data >> 6 & 0x1;
	D7 = data >> 7 & 0x1;
	// send and wait
	E = 1;
	for (i = 0; i < 100; i++);
	E = 0;
	for (i = 0; i < 100; i++);
	
	// lower nibble
	D4 = data & 0x1;
	D5 = data >> 1 & 0x1;
	D6 = data >> 2 & 0x1;
	D7 = data >> 3 & 0x1;
	
	// send and wait
	E = 1;
	for (i = 0; i < 100; i++);
	E = 0;
	for (i = 0; i < 100; i++);	
	sendCmd(0x06);
	return 0;
}

int puts(char *str)
{
	int i = 0;

	while (str[i] != 0)
	{	
		// move to next line on newline char
		if (str[i] == '\n') { i++; sendCmd(0xC0);}
		sendData((int)str[i]);
		i++;	
	}
	return i;
}

int cls()
{
	sendCmd(0x01);	// clear and home command
	return 0;
}

int initAD()
{
	// set AN0 and AN1 as inputs
	TRISAbits.TRISA0 = 1;
	TRISAbits.TRISA1 = 1;
	// make them analog
	AD1PCFGbits.PCFG0 = 0;
	AD1PCFGbits.PCFG1 = 0;
	// reference voltage is Vdd
	AD1CON2bits.VCFG = 0;	

	// derive rate from system clock
	AD1CON3bits.ADRC = 0;
	// set auto sample time to 1 Tad
	AD1CON3bits.SAMC = 1;
	// set conversion clock to 1 Tcy
	AD1CON3bits.ADCS = 1;

	// convert when SAMP bit is cleared
	AD1CON1bits.SSRC = 0;

	// sample data as integer
	AD1CON1bits.FORM = 0;

	// enable A/D
	AD1CON1bits.ADON = 1;

	return 0;
}
