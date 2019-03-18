#include"mbed.h"
#include "i2c.h"
// LCD Commands
// -------------------------------------------------------------------------
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80
// flags for display entry mode
// -------------------------------------------------------------------------
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00
// flags for display on/off and cursor control
// -------------------------------------------------------------------------
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00
// flags for display/cursor shift
// -------------------------------------------------------------------------
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00
// flags for function set
// -------------------------------------------------------------------------
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00
///*********************************************
//Project : I2C to LCD Interface-Routine
//Port PCF8574 : 7 6 5 4 3 2 1 0
// D7 D6 D5 D4 BL EN RW RS
//**********************************************/
#define PCF8574T 0x27
I2C i2c(p9,p10);// SDA, SCL//

DigitalOut clk(p27);
DigitalOut rst(p28);
DigitalOut data(p30);

DigitalOut col1(p14); // blue p0_6
DigitalOut col2(p13); // green p0_7
DigitalOut col3(p12); // yellow p0_8
DigitalOut col4(p11); //orange p0_9

DigitalIn row1(p8, PullUp); // red p0_21
DigitalIn row2(p7, PullUp); // brown p1_20
DigitalIn row3(p6, PullUp); // black p1_23
DigitalIn row4(p5, PullUp); //white p1_24
DigitalIn button(p23, PullUp);
DigitalOut led(p39);

static unsigned char wr_lcd_mode(char c, char mode) {
char ret = 1;
char seq[5];
static char backlight = 8;
if (mode == 8) {
backlight = (c != 0) ? 8 : 0;
return 0;
}
mode |= backlight;
seq[0] = mode; // EN=0, RW=0, RS=mode
seq[1] = (c & 0xF0) | mode | 4; // EN=1, RW=0, RS=1
seq[2] = seq[1] & ~4; // EN=0, RW=0, RS=1
seq[3] = (c << 4) | mode | 4; // EN=1, RW=0, RS=1
seq[4] = seq[3] & ~4; // EN=0, RW=0, RS=1
i2c.start();
i2c.write(PCF8574T << 1);
uint8_t i;
for (i = 0; i < 5; i++) {
i2c.write(seq[i]);
wait(0.002);
}
ret = 0;
i2c.stop();
if (!(mode & 1) && c <= 2)
wait(0.2); // CLS and HOME
return ret;
}
unsigned char lcd_command(char c) {
wr_lcd_mode(c, 0);
}
unsigned char lcd_data(char c) {
wr_lcd_mode(c, 1);
}
void lcd_init(void) {
char i;
// High-Nibble von Byte 8 = Display Control:
// 1DCB**** D: Disp on/off; C: Cursor on/off B: blink on/off
char init_sequenz[] = { 0x33, 0x32, 0x28, 0x0C, 0x06, 0x01 };
wait(1); // Delay power-up
for (i = 0; i < sizeof(init_sequenz); i++) {
lcd_command(init_sequenz[i]);
}
}
void lcd_backlight(char on) {
wr_lcd_mode(on, 8);
}
int decode ( int x)
{
	x = ((x>>4)*10 + (x & 0x0f));
	return x;
}//end of decode function
void printspaces(int space)
{
	for(int i = 0; i < space; i++)
	{lcd_data(' ');}
}//print spaces
void displayDate(I2C i2c)
{
		int sec = 0;
		int min = 0;
		int hrs = 0;
		int day = 0;
		int date = 0;
		int mon = 0;
		int year = 0;
		int secMSB = 0;//need single digits to display
		int secLSB = 0;
		int minMSB = 0;
		int minLSB = 0;
		int hrsMSB = 0;
		int hrsLSB = 0;
		int dateMSB = 0;
		int dateLSB = 0;
		int monMSB = 0;
		int monLSB = 0;
		int yearMSB = 0;
		int yearLSB = 0;

		i2c.start();
		i2c.write(0x90); // address is 1001000 and write is 0
		i2c.write(0xac); // temp sensor
		i2c.write(2);
		i2c.stop();
			
					//start read
		i2c.start();
		i2c.write(0xd0);//address with read as last bit
		i2c.write(0x00);//start are register 0x00
		i2c.start();
		i2c.write(0xd1);//address with a write bit 
		sec = i2c.read(1);//read and acknowledge
		min = i2c.read(1);
		hrs = i2c.read(1);
		day = i2c.read(1);
		date = i2c.read(1);
		mon = i2c.read(1);
		year = i2c.read(0);//don't acknowledge
		i2c.stop();

		secMSB = sec/10 + '0'; //parse and convert seconds
		secLSB = sec%10 + '0';
		
		minMSB = min/10 + '0';
		minLSB = min%10 + '0';
		
		hrsMSB = hrs/10 + '0';
		hrsLSB = hrs%10 + '0';
		
		dateMSB = date/10 + '0';
		dateLSB = date%10 + '0';
		
		monMSB = mon/10 + '0';
		monLSB = mon%10 + '0';
		
		yearMSB = (year/10) + '0';
		yearLSB = (year%10) + '0';
		
		lcd_data(hrsMSB);// show digits starting from the left
		lcd_data(hrsLSB);
		lcd_data(':');
		lcd_data(minMSB);
		lcd_data(minLSB);
		lcd_data('p');
		lcd_data('m');
		lcd_data(' ');
		if(mon == 11)
			{
				lcd_data('N');
				lcd_data('O');
				lcd_data('V');
			}
		lcd_data(' ');
		lcd_data(dateMSB);
		lcd_data(dateLSB);
		lcd_data(',');
		lcd_data('2');
		lcd_data('0');
		lcd_data(yearMSB);
		lcd_data(yearLSB);
		lcd_data(' ');
		
		wait(.1);

}//end of display date and time
void setTime(I2C i2c)
{
		i2c.start();
		i2c.write(0xd0); //clock chip
		i2c.write(0x00);
		i2c.write(0x00);
		i2c.stop();
	
		i2c.start(); //setting up clock
		i2c.write(0xd0);
		i2c.write(0x00);
		i2c.write(0); // sec
		i2c.write(17); //min
		i2c.write(9); //hour
		i2c.write(2); // day
		i2c.write(27); //date
		i2c.write(11); //month
		i2c.write(18); //year
		i2c.write(5);// setting the alarm for 5 seconds after program runs
		i2c.write(17); //min
		i2c.write(9); //hour
		i2c.write(27); //date
	
		i2c.start();
		i2c.write(0xd0);
		i2c.write(0x0e);
		i2c.write(0x20);
		i2c.write(0);
		i2c.stop();
		//end write
}//initialize time
bool checkAlarm(I2C i2c)
{

		i2c.start();
		i2c.write(0xd0);
		i2c.write(0x00);
		i2c.start();
		i2c.write(0xd1);
		int sec = i2c.read(1);// read the time registers and check to see if 
		int min = i2c.read(1);//they are equal to the alarm registers with a tolerance
		int hrs = i2c.read(1);
		int day = i2c.read(1);
		int date = i2c.read(1);
		int mon = i2c.read(1);
		int year = i2c.read(1);
		int almsec = i2c.read(1);
		int almmin = i2c.read(1);
		int almhrs = i2c.read(1);
		int almdate = i2c.read(0);
		i2c.stop();
		int secp = sec + 1;
		int secm = sec - 1;
		
		if (min == almmin && (secp == almsec || secm == almsec || sec == almsec))
		{
			return true;
		}
		else
		{return false;}
}//end of function to check alarm
void displayTemp(I2C i2c)
{
	int temperature = 0;
	
	i2c.start(); //initializing temp sensor
		i2c.write(0x90);
		i2c.write(0x51);
		i2c.stop();
		
		wait(.1);
		
		i2c.start();
		i2c.write(0x90);
		i2c.write(0xaa);
		
		wait(.1);
		
		i2c.start();
		i2c.write(0x91);
		temperature = i2c.read(0); //reading temp
		i2c.stop();
		
		wait(.1);
		
		temperature = (temperature << 8)/256;
		int temp = temperature;
		int tempMSB = temp/10 + '0';
		int tempLSB = temp%10 + '0';
		
		int tempf = temperature * 1.8 + 32;
		int tempfMSB = tempf/10 + '0';
		int tempfLSB = tempf%10 + '0';
		
		printspaces(3);
		lcd_data('T');
		lcd_data('e');
		lcd_data('m');
		lcd_data('p');
		lcd_data(':');
		lcd_data(tempMSB);
		lcd_data(tempLSB);
		lcd_data('C');
		lcd_data('(');
		lcd_data(tempfMSB);
		lcd_data(tempfLSB);
		lcd_data('F');
		lcd_data(')');
		printspaces(44);
		
}//end of display temp

int scanC (DigitalOut c1, DigitalOut c2, DigitalOut c3, DigitalOut c4, DigitalIn r1, \
			DigitalIn r2, DigitalIn r3, DigitalIn r4)
{
			
			if ( r1 == 0) // if button in row 1 is pushed
			{	
				//while (r1 == 0)
				//wait(.01); // this should get rid of the need for waiton function
				if( c1 == 0) // if column one is being scanned
				{return 1;} 
				
				else if( c2 == 0) // if column two is being scanned
				{return 2;}
				
				else if( c3 == 0) // if column three is being scanned
				{return 3;}
				
				else if( c4 == 0) // if column four is being scanned
				{return 11;} // return A
				
			}
			
			
			else if( r2 == 0) // if button in row 2 is pushed
			{
				
				if( c1 == 0) // if column one is being scanned
				{return 4;}
				
				else if( c2 == 0) // if column two is being scanned
				{return 5;}
				
				else if( c3 == 0) // if column three is being scanned
				{return 6;}
				
				else if( c4 == 0) // if column four is being scanned
				{return 12;} // return b
				
			}
			
			else if( r3 == 0) // if button in row 3 is pushed
			{
			
				if( c1 == 0) // if column one is being scanned
				{return 7;}
				
				else if( c2 == 0) // if column two is being scanned
				{return 8;}
				
				else if( c3 == 0) // if column three is being scanned
				{return 9;}
				
				else if( c4 == 0) // if column four is being scanned
				{return 13;} // return C
				
			}
			
			else if( r4 == 0) // if button in row 4 is pushed
			{
				if( c1 == 0) // if column one is being scanned
				{return 0;}
				
				else if( c2 == 0) // if column two is being scanned
				{return 16;} // return F
				
				else if( c3 == 0) // if column three is being scanned
				{return 15;} // return E
				
				else if( c4 == 0) // if column four is being scanned
				{return 14;} // return d
				
			}
			else return 100; // return 100 if no value is read
			
}// end of scan column

int scannum (DigitalOut col1, DigitalOut col2, DigitalOut col3, DigitalOut col4, \
				 DigitalIn row1, DigitalIn row2, DigitalIn row3, DigitalIn row4)
{
	int num = 100;
	
	while( num == 100)
	{
			col1 = 0; // set up columns to scan C1
			col2 = 1;
			col3 = 1;
			col4 = 1;
			
			// sets number equal to value read, number equals 100 if nothing read
			num = scanC(col1, col2, col3, col4, row1, row2, row3, row4);// scan C1
			
			col1 = 1;
			col2 = 0; // set up columns to scan C2

			if( num == 100) // checks to see if the last column read a value
			{	// sets number equal to value read, number equals 100 if nothing read
			num = scanC(col1, col2, col3, col4, row1, row2, row3, row4);//scan C2
			}
			
			col2 = 1;
			col3 = 0; // set up columns to scan C3
			
			if( num == 100)  // checks to see if the last column read a value
			{	// sets number equal to value read, number equals 100 if nothing read
			num = scanC(col1, col2, col3, col4, row1, row2, row3, row4);//scan C3
			}
			
			col3 = 1;
			col4 = 0; // set up columns to scan C4
			
			if( num == 100) // checks to see if the last column read a value
			{	// sets number equal to value read, number equals 100 if nothing read
			num = scanC(col1, col2, col3, col4, row1, row2, row3, row4);//scan C4
			}
			col4 = 1;
	}//end of while this loop runs until a button is pushed
	
	return num;
}//end of scannum this will return the number once found
void disresult(int op, int num1, int num2)
{
		int result = 0;
		int resdig1 = 0;
		int resdig2 = 0;
		int resdig3 = 0;
		int neg = 0;
		
			switch( op )
		{
			case 11:
			{
				result = num1 + num2;
				break;
			}// end of add
			
			case 12:
			{
				result = abs(num1 - num2);
				
				if( num1 - num2 < 0)
				{  neg = 1; }// set a flag for negative value
				break;
				
			}
			
			case 13:
			{
				result = num1 & num2;
				break;
			}
			
			case 14:
			{
				result = num1 | num2;
				break;
			}
		}//end of switch
		
		resdig1 = result % 10;
		result = result - resdig1;
		result = result / 10;
		resdig2 = result % 10;
		result = result - resdig2;
		resdig3 = result / 10;
		
		resdig1 = resdig1 + '0';
		resdig2 = resdig2 + '0';
		resdig3 = resdig3 + '0';
		
		if( neg == 1)
		{
			lcd_data('-');
			lcd_data(resdig3);
			lcd_data(resdig2);
			lcd_data(resdig1);
			lcd_data('.');
			printspaces(5);
			
		}//display negative value
		else
		{
			lcd_data(resdig3);
			lcd_data(resdig2);
			lcd_data(resdig1);
			lcd_data('.');
			printspaces(6);
		}//shift in values if not subtract and negative result
		
}//end of function that displays result

void waiton (int number, DigitalOut col1, DigitalOut col2, \
			DigitalOut col3, DigitalOut col4, \
			 DigitalIn row1, DigitalIn row2, DigitalIn row3, DigitalIn row4)
{
			//int count = 0;
			while( number != 100)//scan column of button pushed until button is let go
			{
					if( number == 1 || number == 4 || number == 7 || number == 0)
					{
						col1 = 0;
						number = scanC(col1, col2, col3, col4, row1, row2, row3, row4);
						col1 = 1;
					}
					
					
					else if( number == 2 || number == 5 || number == 8 || number == 16)
					{
						col2 = 0;
						number = scanC(col1, col2, col3, col4, row1, row2, row3, row4);
						col2 = 1;
					}
					
					
					else if( number == 3 || number == 6 || number == 9 || number == 15)
					{
						col3 = 0;
						number = scanC(col1, col2, col3, col4, row1, row2, row3, row4);
						col3 = 1;
					}
					else if( number == 11 || number == 12 || number == 13 || number == 14)
					{
						col4 = 0;
						number = scanC(col1, col2, col3, col4, row1, row2, row3, row4);
						col4 = 1;
					}
					
			}//end of while that waits for button to be let go (number = 100)
}//end of waiton. Finishes when button is released ( number = 100)

bool calculator(DigitalOut col1, DigitalOut col2, \
			DigitalOut col3, DigitalOut col4, \
			 DigitalIn row1, DigitalIn row2, DigitalIn row3, DigitalIn row4)
{
	int number = 0;
	int num1dig1 = 0;
	int num1dig2 = 0;
	int num2dig1 = 0;
	int num2dig2 = 0;
	int op = 0;
	int count = 1;
	int clear = 1;
	int number1 = 0;
	int number2 = 0;
	char charn1d1;
	char charn1d2;
	char charn2d1;
	char charn2d2;
	bool calc = false;
	
	while(1)
	{
	if(clear == 1)
			{lcd_command(LCD_CLEARDISPLAY);
			clear++;}
			
			number = scannum(col1,col2,col3,col4,row1,row2,row3,row4);
			// sets number equal to value when button is pressed. Keeps running 
			//until a value is read
			wait(.1);

			waiton(number,col1,col2,col3,col4,row1,row2,row3,row4);
			// waits until button is let go. This only returns when button is let go
			wait(.1);
			
		if (count == 1 && number != 11 && number != 12 && number != 13 \
				&& number != 14 && number != 15 && number != 16)
			{
				num1dig1 = number;//number for calculation
				charn1d1 = number + '0';//convert to char to display
				lcd_data(charn1d1);
				count++;
			}// saves number one digit one
			
		else if (count == 2  && number != 11 && number != 12 && number != 13 \
				&& number != 14 && number != 15 && number != 16)
			{
				num1dig2 = number;
				charn1d2 = number + '0';

				//display the number entered
				lcd_data(charn1d2);
				
				number1 = (num1dig1 * 10) + num1dig2;//combines two digits into one value
				count++;
			}// saves number one digit two and displays first number
			
			else if (count == 3  && number != 15 && number != 16 && number != 10 && number != 9 \
			&& number != 8 && number != 7 && number != 6 && number != 5 && number != 4 && number != 3\
			&& number != 2 && number != 1 && number != 0)
			{
				op = number;
				
				lcd_data(' ');
				switch(number)
				{
					case 11:
					lcd_data('+');
					break;
					
					case 12:
					lcd_data('-');
					break;
					
					case 13:
					lcd_data('A');
					break;
					
					case 14:
					lcd_data('O');
					break;
				}//end of switch
				lcd_data(' ');
				count++;
			}//saves operand
			
			else if (count == 4  && number != 11 && number != 12 && number != 13 \
				&& number != 14 && number != 15 && number != 16)
			{
				num2dig1 = number;
				charn2d1 = number + '0';
				lcd_data(charn2d1);
				count++;
			}//saves number two digit one
			
			else if (count == 5 && number != 11 && number != 12 && number != 13 \
				&& number != 14 && number != 15 && number != 16)
			{
				num2dig2 = number;
				charn2d2 = number + '0';
				lcd_data(charn2d2);
				number2 = (num2dig1 * 10) + num2dig2;//combines two digits into one value
				count++;
			}//saves number two digit two
			
			else if ( count == 6 && number == 15 && number != 16)
			{
				lcd_data(' ');
				lcd_data('=');
				lcd_data(' ');
				disresult( op , number1, number2);
			
				count = 1;
			}//end of if that displays result
			wait(.1);
			if( number == 16)
			{
				count = 1;
				return false;
				
			}//exit loop.	
		}//end of while
		
}//end of calculator
int main() {

	lcd_init();
	setTime(i2c);//initialize time
	bool calc = false;//three bool variables to control states
	bool btn = false;
	bool  alarm = false;
	col1 = 1;//Drive all columns high, drive one column low before scanning it
	col2 = 1;
	col3 = 1;
	col4 = 1;
	
	while(1)//main loop that runs forever
	{
	while( calc == false && btn == false && alarm == false)//state0
	{
		lcd_command(LCD_CLEARDISPLAY);
		displayDate(i2c);
		displayTemp(i2c);
	
		col2 = 0;
		if( row4 == 0 && col2 == 0)// check to see if user still pressing button
		{
			calc = true;//set calc equal to true to enter calculator
			while( row4 == 0)
			wait(.01);//wait for user to stop pushing F
		}
		col2 = 1;
		
		if (button == 0)
		btn = true;//check for button, enter check alarm state
		
		alarm = checkAlarm(i2c);//alarm is true when in alarm state
	}//normal display of time date and temperature
	
	while(calc == true && btn == false && alarm == false)
	{
		calc = calculator(col1,col2,col3,col4,row1,row2,row3,row4);
		lcd_command(LCD_CLEARDISPLAY);
		calc = false;

	}// end of calculator
	
	while(calc == false && btn == false && alarm == true)
	{
		led = 1;
		lcd_command(LCD_CLEARDISPLAY);
		lcd_data('A');
		lcd_data('L');
		lcd_data(' ');
		lcd_data('h');
		lcd_data('a');
		lcd_data('s');
		lcd_data(' ');
		lcd_data('e');
		lcd_data('x');
		lcd_data('p');
		printspaces(10);
		
		displayDate(i2c);
		displayTemp(i2c);
		
		if(button == 0)
		{
			alarm = false;//reset alarm
			while(button == 0)
			{
				wait(.01);
			}
		}//user pressed button, reset alarm
		wait(.2);
		led = 0;
		wait(.2);	
		
	}//end of alarm
	
	while(calc == false && btn == true && alarm == false)
	{
		while(button == 0)
		{
			wait(.1);
		}
		
		lcd_data('C');
		lcd_data('u');
		lcd_data('r');
		lcd_data('r');
		lcd_data(' ');
		lcd_data('A');
		lcd_data('l');
		lcd_data('m');
		printspaces(12);
		lcd_data('9');
		lcd_data(':');
		lcd_data('1');
		lcd_data('8');
		lcd_data('p');
		lcd_data('m');
		lcd_data(' ');
		lcd_data('N');
		lcd_data('O');
		lcd_data('V');
		lcd_data(' ');
		lcd_data('2');
		lcd_data('7');
		lcd_data(',');
		lcd_data(' ');
		lcd_data('2');
		lcd_data('0');
		lcd_data('1');
		lcd_data('8');
		wait(.4);
		lcd_command(LCD_CLEARDISPLAY);
		
	if(button == 0)
		btn = false;

			while(button == 0)
		{
			wait(.1);
		}
		
	}//end of check alarm
}//end of main while
  
}//end of main

