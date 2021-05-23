#include <MKL25Z4.H>
#include <String.h> // supports string stuff
#include <stdio.h>
#include <stdlib.h>


#define RS 0x01 
#define RW 0x04 
#define EN 0x08	 


// Operations Modes
void manualMode(void);
void automaticMode(void);
void emergencyMode(void);
// Sellections
char selectMode(void);
void selectDutyCycle(void);


// time funcitons
void delayMs(int n);
void delayUs(int n);
void clocksInit(void);


// PWM INIT
void PWM_INIT(int cnV);
int getVoltageLecture(void);
void autoInterval(int volt);



// ADC
void ADC_INIT(void);


// interruption pins
void inputPinsInit(void);

//  LCS PRROTOYPES
void LCD_nibble_write(unsigned char data, unsigned char control);
void LCD_command(unsigned char command);
void LCD_data(unsigned char data);
void LCD_init(void);
void printStuff(char * line, int len);
void lcdReset(void);
void lcdBegin(void); // Sets the lcd to print
void writeKey(char key); // Writes: "Selected: key"
void emergencyWrite(void);
void printLecture(int num);



// Print Menu Fuctions
void modeMenu(void); // First Menu
void dutyCicleMenu(void); // Duty Cicle Meny

// KeypadPrototypes
void keypad_init(void); // init
char keypad_getkey(void); // getKey
int getNumber(void);
void writeSelectNum(void);
void append(char* s, char c);
int getNumber(void);
 
 
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
	int volt;
	volt = getVoltageLecture();
	printLecture(volt);
	autoInterval(volt);

	

}

void autoInterval(int volt){
	if((volt >= 0)&&(volt <= 16783)){
		__disable_irq();
		PWM_INIT(10925);
		__enable_irq();

	}
	else if((volt >= 16784)&&(volt <= 32767)){
		__disable_irq();
		PWM_INIT(21851);
		__enable_irq();
	}
	else if((volt >= 32768)&&(volt <= 49151)){
		__disable_irq();
		PWM_INIT(32777);
		__enable_irq();
	}
	else{
		__disable_irq();
		PWM_INIT(43703);
		__enable_irq();

	}

}


void emergencyMode(void){
	char key;
	__disable_irq();
	PWM_INIT(43703);
	PTD->PTOR = 0x01;
	__enable_irq();
	do{
		emergencyWrite();
		key = keypad_getkey();
	}while(key != '#');
	__disable_irq();
	PTD->PTOR = 0x01;
	PWM_INIT(10925);
	__enable_irq();

}



// Select Menus

char selectMode(void){
	char mode = ' ';
	while(mode != '1' && mode != '2'){
		modeMenu();
		mode = keypad_getkey();
	}
	writeKey(mode);
	return mode;
}


void selectDutyCycle(void){
	char key;
	do{
		dutyCicleMenu();
		key = keypad_getkey();
	}
	while(key != '1' && key != '2' && key != '3' && key != '4');
	writeKey(key);
	lcdReset();
	if(key == 1){
		PWM_INIT(10925);
	}
	else if (key == '2'){
		PWM_INIT(21851);
	}
	else if(key == '3'){
		PWM_INIT(32777);
	}
	else if(key == 4){
		PWM_INIT(43703);
	}
	else{
		PWM_INIT(10925);
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



void PWM_INIT(int cnV){
	PORTD->PCR[1] = 0x0400; // enable TMP0 to Pin 1
	SIM->SCGC6 |= 0x01000000; // enable clock to tpm 0
	SIM->SOPT2 |= 0x01000000; // 41.96 HZ clock

	TPM0->SC = 0; // enable timer
	TPM0->CONTROLS[1].CnSC = 0x20 | 0x08;  // enable timer (counter)
	TPM0->MOD = 43702; // MODULE
	TPM0->CONTROLS[1].CnV = cnV; // COUNTER (WIDTH)
	TPM0->SC = 0x0C;    // Enable with prescaler = 16
}



// initialize adc0 to enable date receotion in pte20
void ADC_INIT(void){
	PORTE->PCR[20] = 0; 
	SIM->SCGC6 |= 0x8000000; 
	ADC0->SC2 &= ~0x40; 
	ADC0->SC3 |= 0x07; 
	ADC0->CFG1 = 0x40 | 0x10 | 0x0C | 0x00;

}

int getVoltageLecture(void){
	int result;
	ADC0->SC1[0] = 0;
	while(!(ADC0->SC1[0] & 0x80)) { }
	result = ADC0->R[0]; 
	return result;
}



// Interruotion pins
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
LCD_nibble_write(0x20, 0); 
delayMs(1);
LCD_command(0x28); 
LCD_command(0x06); 
LCD_command(0x01); 
LCD_command(0x0F); 

}



void LCD_nibble_write(unsigned char data, unsigned char control)
{
data &= 0xF0; 

control &= 0x0F; 

PTD->PDOR = data | control; 
PTD->PDOR = data | control | EN; 

delayMs(0);
PTD->PDOR = data;
//PTD->PDOR|= 0xFD;
PTD->PDOR = 0;
PTD->PSOR |= 0x02; 

}






void LCD_command(unsigned char command)
{

LCD_nibble_write(command & 0xF0, 0); 


LCD_nibble_write(command << 4, 0); 

if (command < 4)

delayMs(4); 
else
delayMs(1); 
}



void LCD_data(unsigned char data)
{
LCD_nibble_write(data & 0xF0, RS);
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
		lcdBegin();
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
	char r1[] = "Select a";
	char r2[] = "number:";
	lcdBegin();
	printStuff(r1,strlen(r1));
	LCD_command(0xC0);
	printStuff(r2,strlen(r2));
	lcdReset();

} // Dysplays "Select a Number"



void emergencyWrite(void){
	char r1[] = "Emergency Stop";
	char r2[] ="# to resume";
	LCD_command(1); 
	delayMs(500);
	LCD_command(0x80);
	printStuff(r1,strlen(r1));
	LCD_command(0xC0);
	printStuff(r2,strlen(r1));
	lcdReset();

}



// this funciton actually stayed in the ancient fornmat
void printLecture(int num){
	char str[1];
	int i;
	int len;
	sprintf(str,"%d",num);
	len = strlen(str);
	lcdBegin();
	LCD_data('V'); 
	LCD_data('o');
	LCD_data('l');
	LCD_data('t');
	LCD_data('a');
	LCD_data('g'); 
	LCD_data('e');
	LCD_data(':');
	LCD_command(0xC0);
	LCD_data(' ');		
	for(i = 0; i < len ; i ++){
		LCD_data(str[i]);
	}
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


PTC->PDDR |= 0x0F; 
PTC->PCOR = 0x0F;
delayUs(2); 
col = PTC-> PDIR & 0xF0; 
PTC->PDDR = 0;
if (col == 0xF0)
return 0; 

for (row = 0; row < 4; row++)
{ PTC->PDDR = 0; 

PTC->PDDR |= row_select[row]; 
PTC->PCOR = row_select[row]; 
delayUs(2); 
col = PTC->PDIR & 0xF0; 

if (col != 0xF0) break; 
}


PTC->PDDR = 0; 
PTD->PSOR |= 0x02; 

if (row == 4)
return 0; 



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
return 0; 
}

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



int main(void){
	char mode = '1';
	__disable_irq();
	clocksInit();
	keypad_init();
	LCD_init();
	ADC_INIT();
	inputPinsInit();
	mode = selectMode();
	if(mode == '1'){
		selectDutyCycle();
	}
	delayMs(1000);
	NVIC->ISER[0] |= 0x40000000;
	__enable_irq(); // enable interrupts again
	while(1){
		if(mode == '2'){
			automaticMode();
		}
	}
}



void PORTA_IRQHandler(void) {
	int i;
	while (PORTA->ISFR & 0x00000006) { // check for possible ISFR bits active (of the ones associated with each push button)
		if (PORTA->ISFR & 0x00000002) { // Secondt Bit 
		emergencyMode();

			
		PORTA->ISFR = 0x00000002; // clear interrupt setvice flag
		}


		if (PORTA->ISFR & 0x00000004) { // third bit
			__disable_irq();
			selectDutyCycle();
			__enable_irq();
			PORTA->ISFR = 0x00000004; // clear interrupt setvice flag
		} 
	} 
}
