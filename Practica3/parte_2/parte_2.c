// questo programa fa lo stesso che faceva l´ altro ma invece di usare 8 pins di dati ne prende solamente 4 piu quei di controllo

/*
opcion 1
strcat(num,&key);
opcion 2

// questo programa fa lo stesso che faceva l´ altro ma invece di usare 8 pins di dati ne prende solamente 4 piu quei di controllo

/*
opcion 1
strcat(num,&key);
opcion 2
len = strlen(num);
snprintf(num+len,sizeof num-len,"%c",key);
*/

#include <MKL25Z4.h>
#include <String.h> // supports string stuff
#include <stdio.h>
#include <stdlib.h>




#define RS 0x01 /* BIT0 mask */
#define RW 0x04 /* BIT2 mask */
#define EN 0x08	 /* BIT3 mask */

// time funcitons
void delayMs(int n);
void delayUs(int n);

// LDC Prototpyes

void LCD_nibble_write(unsigned char data, unsigned char control);
void LCD_command(unsigned char command);
void LCD_data(unsigned char data);
void LCD_init(void);
void writeKey(char key);
void oli(void); // gretting
void writeSelectNum(void); // Dysplays "Select a Number"
void kenobi(void);//important meme
int getNumber(); // retieves a number from the keypad
void writeTime(int time);
void append(char* s, char c);

// keypad prototypes:
void keypad_init(void);
char keypad_getkey(void);


// LED init
void ledsInit(void);
void toogleLed(char key);


// Clock init
void clocksInit(void);

// timer prototypes
void timerInit(void);




// Time functions
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



void timerInit(void){
	SIM->SCGC6 |= 0x02000000; /* enable clock to TPM0 */
	SIM->SOPT2 |= 0x01000000;/* use MCGFLLCLK as CNT clock */
	TPM1->SC = 0; /* disable timer while configuring */
	TPM1->SC = 0x07; 
	TPM1->MOD = 0xFFFF; /* max modulo value */
	TPM1->SC |= 0x80; /* clear TOF */
	TPM1->SC |= 0x08; /* enable timer free-running mode */
}



void delayOneSec(void){
	int j;
	for(j = 0; j < 1000000; j++){
		// waits for one second
		while((TPM1->SC & 0x80) == 0) { } 
	}
}


void countdown(int num){
	while(num > 0){
		writeTime(num);
		delayOneSec();
		//delayMs(1000);
		num--;
	}
	LCD_command(0x01);

}


/*
void countdown(int num){
	int i;
	while(num > 0){
		writeTime(num);
		for(i = 0; i < 640; i++){
			while((TPM1->SC & 0x80) == 0) { 
			}			
		}
		num--;
	}
	LCD_command(0x01);
}
*/


/*
void delaysOneSec(int n){
	int countdown = n;
	int i,j;
	// repeats n one second cicles
	for(i = 0; i < n; i++){
		// creates one second delay
		for(j = 0; j < 640; j++){
			// waits for one second
			while((TPM1->SC & 0x80) == 0) { } 
			writeTime(countdown);
		}
		// dysplay action
		countdown -=1;
		
		
	}
}
*/
void append(char* s, char c) {
	int len = strlen(s);
	s[len] = c;
	s[len+1] = '\0';
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






void writeTime(int time){
	char str[1];
	int i;
	int len;
	sprintf(str,"%d",time);
	len = strlen(str);
	LCD_command(1); 
	delayMs(500);
	LCD_command(0x80);
	LCD_data('C');
	LCD_data('o');
	LCD_data('u');
	LCD_data('n');
	LCD_data('t');
	LCD_data('d');
	LCD_data('o');
	LCD_data('w');
	LCD_data('n');
	LCD_data(':');
	LCD_command(0xC0);
	LCD_data(' ');
	LCD_data(' ');
	LCD_data('*');
	LCD_data('*');
	LCD_data('*');
	for(i = 0; i < len ; i ++){
		LCD_data(str[i]);
	}
	LCD_data('*');
	LCD_data('*');
	LCD_data('*');


}








void clocksInit(){
	SIM->SCGC5 |= 0x1000; // enable to port D
	SIM->SCGC5 |= 0x400; // ENABLE CLOCK TO PORT B

}










// Initalize leds
void ledsInit(void){
	// MAKE THREE MISCLEANEOUS LEDS con pull down resitors
	PORTD->PCR[1] = 0x100;
	PORTB->PCR[18] = 0x100;
	PORTB->PCR[19] = 0x100;
	
	
	// ENABLE GPIOS AS OUTPUT PINS
	PTB->PDDR = 0xC0000;
	PTD->PDDR = 0x02;
	
	

	PTB->PSOR |= 0xC0000; 
	PTD->PSOR |= 0x02; 

}









// Keypad funcitons

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



// this basically writes the function 
void oli(void){
		LCD_command(1); 
		delayMs(500);
		LCD_command(0x80);
		LCD_data('H'); 
		LCD_data('e');
		LCD_data('l');
		LCD_data('l');
		LCD_data('o');	
		LCD_data(' ');
		LCD_data('T');
		LCD_data('h');
		LCD_data('e');
		LCD_data('r');		
		LCD_data('e'); 
		LCD_data('!'); 


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

void kenobi(void){
		LCD_command(1); 
		delayMs(500);
		LCD_command(0x80);
		LCD_data('G'); 
		LCD_data('e');
		LCD_data('n');
		LCD_data('e');
		LCD_data('r');		
		LCD_data('a');
		LCD_data('l');
	
		LCD_data(' ');
	
		LCD_data('K');
		LCD_data('e');
		LCD_data('n'); 
		LCD_data('o');
		LCD_data('b');
		LCD_data('i'); 
		LCD_data('!');

}//important meme



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









int main(void)
{
	int num=0;
	clocksInit();
	ledsInit();
	keypad_init();
	LCD_init();
	while(1)
		{
			oli();
			delayMs(2500);
			LCD_command(0x01);
			LCD_command(0x01);
			num = getNumber();			
			delayMs(500);
			LCD_command(0x01);
			timerInit();
			countdown(num);
			
		}
	
}
