/*
This programs presents a little upgrade from the first one. This enables the program to distinguish between diferent inpterrupt request
associated with diferent push buttons XD.
We associate diferent bits in Interrupt Status Flag  to each push button
*/

#include <MKL25Z4.h>

// Usefull for creating ineficient delays
void delayMs(int n);

// Set up the parameter ofthe
void ledInit(void); 

// input pins init
void inputPinsInit(void);

void ledInit(void){
	
	// Port B (Green & Red)
	SIM->SCGC5 |= 0x400; // enable clock to port B
	PORTB->PCR[19] = 0x100; // GREEN LED AS miscelaneus
	PORTB->PCR[18] = 0x100; // RED LED AS miscelaneus
	PTB->PDDR |= 0xC0000; // Make leds as ouput pins
	PTB->PDOR |= 0xC0000; // turn them off

	
	// POrt D (blue)
	SIM->SCGC5 |= 0x1000; // enable clock to port D
	PORTD->PCR[1] = 0x100; // miscelaneus
	PTD->PDDR |= 0x02; // blue led as output pin
	PTD->PDOR |= 0x02; // turn off Led
	

	

	
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



int main(void) {

	__disable_irq(); // disable global interrupts while initializing stuff
	ledInit();
	inputPinsInit();
	NVIC->ISER[0] |= 0x40000000; /* enable INT30 (bit 30 of ISER[0]) */
	__enable_irq(); // enable interrupts again
	while(1) {   // negate the current RED LED´S value forever every 0.5 seconds
		PTB->PTOR |= 0x40000;
		delayMs(500);
	} 
}



/*
Once we enable the interrups in the pin, we care free to use the startUP_MKL25Z 
interrupt functions freely, so we overwrite the one corresponding to PORT A´s interruption requests
*/
void PORTA_IRQHandler(void) {
	int i;
	while (PORTA->ISFR & 0x00000006) { // check for possible ISFR bits active (of the ones associated with each push button)
		if (PORTA->ISFR & 0x00000002) { // Secondt Bit 
			for (i = 0; i < 3; i++) { // toogle green led 3 times
			PTB->PDOR &= ~0x80000; // ON
			delayMs(500);
			PTB->PDOR |= 0x80000; // OF
			delayMs(500);
		}
		PORTA->ISFR = 0x00000002; // clear interrupt setvice flag
		}


		if (PORTA->ISFR & 0x00000004) { // third bit
			for (i = 0; i < 3; i++) { // toogle blue les three times
				PTD->PDOR &= ~0x02; // on
				delayMs(500);
				PTD->PDOR |= 0x02; // off
				delayMs(500);
			}
		PORTA->ISFR = 0x00000004; // clear interrupt setvice flag
		} 
	} 
}



// Time functions
void delayMs(int n) {
	int i;
	int j;
	for(i = 0 ; i < n; i++)
	for(j = 0 ; j < 7000; j++) { }
}



