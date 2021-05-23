  
#include <MKL25Z4.H>
#include <String.h> // supports string stuff
#include <stdio.h>
#include <stdlib.h>


#define RS 0x01 /* BIT0 mask */
#define RW 0x04 /* BIT2 mask */
#define EN 0x08	 /* BIT3 mask */


// Operations Modes
void manualMode(void);
void automaticMode(void);
// Sellections
void selectMode(void);


// time funcitons
void delayMs(int n);
void delayUs(int n);
void clocksInit(void);


//  LCS PRROTOYPES
void LCD_nibble_write(unsigned char data, unsigned char control);
void LCD_command(unsigned char command);
void LCD_data(unsigned char data);
void LCD_init(void);
void printStuff(char * line, int len);
void lcdReset(void);
void lcdBegin(void); // Sets the lcd to print
void writeKey(char key); // Writes: "Selected: key"

// Print Menu Fuctions
void modeMenu(void); // First Menu
void dutyCicleMenu(void); // Duty Cicle Meny

// KeypadPrototypes
void keypad_init(void); // init
char keypad_getkey(void); // getKey


// Initialize Clock
void clocksInit(void){
	SIM->SCGC5 |= 0x1000; // enable to port D
	SIM->SCGC5 |= 0x400; // ENABLE CLOCK TO PORT B
	SIM->SCGC5 |= 0x2000; 
}



// OPERATION MODES 
void manualMode(){
	while(1){
		lcdBegin();
		LCD_data('M');
		lcdReset();
	}
}


void automaticMode(void){
	while(1){
		lcdBegin();
		LCD_data('A');
		lcdReset();
	}
}
// Select Menus

void selectMode(void){
	char key = ' ';
	do{  // select an operating mode
		modeMenu();
		key = keypad_getkey();
	}while((key != '1') || (key != '2'));
	writeKey(key);
	if(key == '1'){
		manualMode();
	}
	else{
		automaticMode();
	}
}


// Menu Dysplay
void modeMenu(void){
	char str1[] = "Manual: 1";
	char str2[] = "Auto: 2";
	lcdBegin();
	printStuff(str1,strlen(str1));
	LCD_command(0xC0);
	printStuff(str2,strlen(str2));
	lcdReset();
 }

 void dutyCicleMenu(void){
	char str1[] = "1:L 2:M 3:MH 4:H";
	lcdBegin();
	printStuff(str1,strlen(str1));
 	lcdReset();
}


// time functions

void delayMs(int n) {
	int i;
	int j;
	for(i = 0 ; i < n; i++)
	for(j = 0 ; j < 7000; j++) { }
}

void delayUs(int n) {
	int i;
	int j;
	for(i = 0 ; i < n; i++)
	for (j = 0; j < 8000; j++) {}
}




// LCD Functions
void LCD_init(void)
{
PORTD->PCR[0] = 0x100; 
//PORTD->PCR[1] = 0x100; // Blue led 
PORTD->PCR[2] = 0x100; 
PORTD->PCR[3] = 0x100; 
PORTD->PCR[4] = 0x100; 
PORTD->PCR[5] = 0x100; 
PORTD->PCR[6] = 0x100; 
PORTD->PCR[7] = 0x100; 

PTD->PDDR |= 0xFD; 

delayMs(30); 
delayMs(10);
delayMs(1);

delayMs(1);
LCD_nibble_write(0x20, 0); /* use 4-bit data mode */
delayMs(1);
LCD_command(0x28); /* set 4-bit D, 2-line, 5x7 font */
LCD_command(0x06); /* move cursor right */
LCD_command(0x01); /* clr screen, move cursor home */
LCD_command(0x0F); /* turn on display, cursor blink*/

}



void LCD_nibble_write(unsigned char data, unsigned char control)
{
data &= 0xF0; /* clear lower nibble for control */

control &= 0x0F; /* clear upper nibble for data */

PTD->PDOR = data | control; /* RS = 0, R/W = 0 */
PTD->PDOR = data | control | EN; /* pulse E */

delayMs(0);
PTD->PDOR = data;
//PTD->PDOR|= 0xFD;
PTD->PDOR = 0;
PTD->PSOR |= 0x02; 

}






void LCD_command(unsigned char command)
{
/* upper nibble first */

LCD_nibble_write(command & 0xF0, 0); 

/* then lower nibble */

LCD_nibble_write(command << 4, 0); 

if (command < 4)

delayMs(4); /* commands 1 and 2 need up to 1.64ms */
else
delayMs(1); 
}



void LCD_data(unsigned char data)
{/* upper nibble first */
LCD_nibble_write(data & 0xF0, RS);
/* then lower nibble */ 
LCD_nibble_write(data << 4, RS); 
delayMs(1);
}


// print a string
void printStuff(char * line, int len){
	int i;
	for(i = 0;i < len; i++){
		LCD_data(line[i]);
	}
}


// clears the ldc
void lcdReset(void){
	 delayMs(1000);
	 LCD_command(0x01);
}

void lcdBegin(void){
	LCD_command(1); 
	delayMs(500);
	LCD_command(0x80);
}

void writeKey(char key){
		char r1[] = "selected";
		lcdBegin();
		printStuff(r1,strlen(r1));
		LCD_data(key);
		lcdReset();

}



void keypad_init(void)
{
SIM->SCGC5 |= 0x0800;  /* enable clock to Port C */
PORTC->PCR[0] = 0x103; /* PTD0, GPIO, enable pullup*/
PORTC->PCR[1] = 0x103; /* PTD1, GPIO, enable pullup*/
PORTC->PCR[2] = 0x103; /* PTD2, GPIO, enable pullup*/
PORTC->PCR[3] = 0x103; /* PTD3, GPIO, enable pullup*/
PORTC->PCR[4] = 0x103; /* PTD4, GPIO, enable pullup*/
PORTC->PCR[5] = 0x103; /* PTD5, GPIO, enable pullup*/
PORTC->PCR[6] = 0x103; /* PTD6, GPIO, enable pullup*/
PORTC->PCR[7] = 0x103; /* PTD7, GPIO, enable pullup*/
PTD->PDDR = 0x0F; /* make PTD7-0 as input pins */
}



char keypad_getkey(void) {

int row, col;
const char row_select[] = {0x01, 0x02, 0x04, 0x08}; 
/* one row is active */
/* check to see any key pressed */

PTC->PDDR |= 0x0F; /* enable all rows */
PTC->PCOR = 0x0F;
delayUs(2); /* wait for signal return */
col = PTC-> PDIR & 0xF0; /* read all columns */
PTC->PDDR = 0; /* disable all rows */
if (col == 0xF0)
return 0; /* no key pressed */

for (row = 0; row < 4; row++)
{ PTC->PDDR = 0; /* disable all rows */

PTC->PDDR |= row_select[row]; /* enable one row */
PTC->PCOR = row_select[row]; /* drive active row low*/

delayUs(2); /* wait for signal to settle */
col = PTC->PDIR & 0xF0; /* read all columns */

if (col != 0xF0) break; 
/* if one of the input is low, some key is pressed. */
}


PTC->PDDR = 0; /* disable all rows */
PTD->PSOR |= 0x02; 

if (row == 4)
return 0; /* if we get here, no key is pressed */

/* gets here when one of the rows has key pressed*/ 
/*check which column it is*/

if (col == 0x70){
	switch(row){
		case 0:
			return 'A';
		case 1:
			return 'B';
		case 2:
			return 'C';
		case 3:
			return 'D';
		default:
			return 0;
	}


} 
if (col == 0xB0){
	switch(row){
		case 0:
			return '3';
		case 1:
			return '6';
		case 2:
			return '9';
		case 3:
			return '#';
		default:
			return 0;
	}

}	
if (col == 0xD0){
	switch(row){
		case 0:
			return '2';
		case 1:
			return '5';
		case 2:
			return '8';
		case 3:
			return '0';
		default:
			return 0;
	}

}
if (col == 0xE0){
	switch(row){
		case 0:
			return '1';
		case 1:
			return '4';
		case 2:
			return '7';
		case 3:
			return '*';
		default:
			return 0;
	}

} 
return 0; /* just to be safe */
}





int main(void){
	clocksInit();
	LCD_init();
	keypad_init();
	selectMode();
}



