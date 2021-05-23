#include <MKL25Z4.H>
#include <String.h> // supports string stuff
#include <stdio.h>
#include <stdlib.h>



#define RS 0x01 /* BIT0 mask */
#define RW 0x04 /* BIT2 mask */
#define EN 0x08	 /* BIT3 mask */


// function prototypes


// time funcitons
void delayMs(int n);
void delayUs(int n);


// keypad prototypes:
void keypad_init(void); // init
char keypad_getkey(void); // getKey
int getNumber(); // retieves a number from the keypad
void append(char* s, char c); // array

// ADC prototypes 
void ADC_INIT(void);
//const char * getTemperatureLecture();
float getTemperatureLecture(void);



// LCD PROTORPYES
void LCD_nibble_write(unsigned char data, unsigned char control);
void LCD_command(unsigned char command);
void LCD_data(unsigned char data);
void LCD_init(void);
void writeKey(char key);
void writeSelectNum(void); // Dysplays "Select a Number"
void menuTemp(void); // writes "Select Target \nTemperature"
void printStuff(char * line, int len);
void printTarget(int num);
void iddleWrite(void); // writes iddle mode

// Clock init
void clocksInit(void);

// timer prototypes
void timerInit(void);
void delayOneSec(void);
void xSecondsDelay(int num); // creates a dekay of x Seconds
void xTremeMode();

// input pins init
void inputPinsInit(void); 
void iddleMode(void);

// fan signal init
void fanInit(void);

// Timer init
void timerInit(void){
	
	SIM->SCGC6 |= 0x02000000; // enable clock to TPM0 
	SIM->SOPT2 |= 0x01000000; //use MCGFLLCLK as CNT clock //
	TPM1->SC = 0; // disable timer while configuring 
	TPM1->SC = 0x07; 
	TPM1->MOD = 0xFFFF; // max modulo value 
	TPM1->SC |= 0x80; // clear TOF 
	TPM1->SC |= 0x08; // enable timer free-running mode 
	
	/*
	we did try to do it the most eficient way possible by using a 32 khz clock, however, this clock does not work since assigment 2 :C
	// timer two for larger delays
	SIM->SCGC6 |= 0x02000000; 
	SIM->SOPT2 |= 0x03000000;
	TPM1->SC = 0; 
	//TPM1->SC = 0x07; // prescales by 128
	TPM1->MOD = 0xFFFF; 
	TPM1->SC |= 0x80; 
	TPM1->SC |= 0x08; 
	*/
}




// fan signal init
void fanInit(void){
	PORTB->PCR[10] = 0x0100; // make pin PTB10 as GPIO
  FPTB->PDOR = 0x400; // switch Red/Green LED off
  FPTB->PDDR = 0x400; // enable PTB18/19 as Output

}


// initialize adc0 to enable date receotion in pte20
void ADC_INIT(void){
	PORTE->PCR[20] = 0; 
	SIM->SCGC6 |= 0x8000000; 
	ADC0->SC2 &= ~0x40; 
	ADC0->SC3 |= 0x07; 
	ADC0->CFG1 = 0x40 | 0x10 | 0x0C | 0x00;

}





// clocks init
void clocksInit(){
	SIM->SCGC5 |= 0x1000; // enable to port D
	SIM->SCGC5 |= 0x400; // ENABLE CLOCK TO PORT B
	SIM->SCGC5 |= 0x2000; 
}


void inputPinsInit(void){
	SIM->SCGC5 |= 0x200; // enable clock to clock to port A
	
	// Set up PTA1
	PORTA->PCR[1] |= 0x103; // miscelaneus pin with PULL UP
	PTA->PDDR &= ~0x0002; // INPUT PIN 1101;
	PORTA->PCR[1] &= ~0xF0000; // Crear Interrupt 16-19 pins input value 0000
	PORTA->PCR[1] |= 0xA0000; // Enable interrupt on falling edge Pints 16-19 1010
	// Set up PTA2	
	PORTA->PCR[2] |= 0x103; // miscelaneus pin with PULL UP
	PTA->PDDR &= ~0x0004; // INPUT PIN 1011;
	PORTA->PCR[2] &= ~0xF0000; // Crear Interrupt 16-19 pins input value 0000
	PORTA->PCR[2] |= 0xA0000; // Enable interrupt on falling edge Pints 16-19 1010
}






// onse second delay
void delayOneSec(void){
	int j;
	for(j = 0; j < 1000000; j++){
		// waits for one second
		while((TPM1->SC & 0x80) == 0) { } 
	}
}



void xSecondsDelay(int num){
	int i;
	for(i = 0; i < num; i++){
		delayOneSec();
	}
}
// Keypad Functions


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


int getNumber(void){
	char num[] = ""; 
	char key; 
	int time;
	do{
		writeSelectNum();
		delayMs(200);
		key = keypad_getkey();
		if(key != 0 && key != 'A'){
			append(num,key);
			writeKey(num[strlen(num)-1]);
			delayMs(500);
			LCD_command(0x01);
		}
	}while(key != 'A');
	time = atoi(num);
	return time;
	
}



void append(char* s, char c) {
	int len = strlen(s);
	s[len] = c;
	s[len+1] = '\0';
}



// lcd funcitons
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


void writeKey(char key){
		LCD_command(1); 
		delayMs(500);
		LCD_command(0x80);
		LCD_data('S'); 
		LCD_data('e');
		LCD_data('l');
		LCD_data('e');
		LCD_data('c');		
		LCD_data('t'); 
		LCD_data('e');
		LCD_data('d');
		LCD_data(':');
		LCD_data(key);

}

void writeSelectNum(void){
		LCD_command(1); 
		delayMs(500);
		LCD_command(0x80);
		LCD_data('S'); 
		LCD_data('e');
		LCD_data('l');
		LCD_data('e');
		LCD_data('c');		
		LCD_data('t');
	
		LCD_command(0xC0);
		LCD_data('a');
		LCD_data(' ');
		LCD_data('n'); 
		LCD_data('u');
		LCD_data('m');
		LCD_data('b');
		LCD_data('e');		
		LCD_data('r'); 
		LCD_data(':');

} // Dysplays "Select a Number"



// clears the ldc
void lcdReset(void){
	 delayMs(1000);
	 LCD_command(0x01);
}

void printStuff(char * line, int len){
	int i;
	for(i = 0;i < len; i++){
		LCD_data(line[i]);
	}
}


void menuTemp(void){
	char r1[] = "Select Target";
	char r2[] = "Temperature:";

	LCD_command(1); 
	delayMs(500);
	LCD_command(0x80);
	printStuff(r1,strlen(r1));
	LCD_command(0xC0);
	printStuff(r2,strlen(r2));
	
}
void printTarget(int num){
	char r1[] = "Target:";
	char str[] ="";
	sprintf(str,"%d",num);
	LCD_command(1); 
	delayMs(500);
	LCD_command(0x80);
	printStuff(r1,strlen(r1));
	LCD_command(0xC0);
	printStuff(str,strlen(str));
	lcdReset();

}

void writeStMeasure(void){
	char r1[] ="# to measure";
	LCD_command(1); 
	delayMs(500);
	LCD_command(0x80);
	printStuff(r1,strlen(r1));
	lcdReset();

}



void printLecture(float num){
	char r1[] = "Lecture:";
	char str[16] = "";
	LCD_command(1); 
	delayMs(500);
	LCD_command(0x80);
	sprintf(str, "%6.2iF", num); 
	printStuff(r1,strlen(r1));
	LCD_command(0xC0);
	printStuff(str,strlen(str));
	lcdReset();
}


void iddleWrite(void){
	char r1[] = "Iddle Mode";
	char r2[] ="# to resume";
	LCD_command(1); 
	delayMs(500);
	LCD_command(0x80);
	printStuff(r1,strlen(r1));
	LCD_command(0xC0);
	printStuff(r2,strlen(r1));
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


float getTemperatureLecture(void){
	int result = 0;
	float temperature;
	ADC0->SC1[0] = 0;
	while(!(ADC0->SC1[0] & 0x80)) { }
	result = ADC0->R[0]; 
	temperature = result * 330.0 / 65536; 
	return temperature;
}


void iddleMode(void){
	char key;
	PTB->PTOR = 0x400;
	do{
		iddleWrite();
		key = keypad_getkey();
	}while(key != '#');
}

void nSecondsFan(int sec){
	FPTB->PCOR = 0x400;
	xSecondsDelay(sec);
	FPTB->PSOR = 0x400;
}


void xTremCooldown(){ // prototrype pending
	char message[] = "Xtreme Cooldown";
	LCD_command(1); 
	delayMs(500);
	LCD_command(0x80);
	printStuff(message,strlen(message));
	lcdReset();
	nSecondsFan(40);
}


void startMeasure(int target){
	char key;
	printTarget(target);
	do{
		writeStMeasure();
		key = keypad_getkey();
	}while(key != '#');
	
}

int main (void) {
	int i = 0;
	int threshold = 5;
	int num = 0;
	float temp = 0;
	__disable_irq();
	clocksInit();
	keypad_init();
	LCD_init();
	inputPinsInit();
	fanInit();
	timerInit();
	ADC_INIT();
	delayMs(1000);
	NVIC->ISER[0] |= 0x40000000;
	menuTemp();
	delayMs(1000);
	LCD_command(0x01);	
	num = getNumber();
	startMeasure(num);
 	__enable_irq(); // enable interrupts again

	while(1) {
	  temp = getTemperatureLecture();
	  printLecture(temp);
		if((temp - num) > threshold){
			xTremCooldown();
		}
		else{
			nSecondsFan(10);
			xSecondsDelay(30);
		}

 }
}




/*
Once we enable the interrups in the pin, we care free to use the startUP_MKL25Z 
interrupt functions freely, so we overwrite the one corresponding to PORT A´s interruption requests
*/
void PORTA_IRQHandler(void) {
	iddleMode();
	PORTA->ISFR = 0x00000006; // clear interruption
	

}


