// This program uses interruptinons to toggle the rgb led to a different color when a push button connected to PTA 1 ispressed.


#include <MKL25Z4.h>

// Usefull for creating ineficient delays
void delayMs(int n);

// Set up the parameter ofthe
void ledInit(void); 

// input pins init
void inputPinsInit(void);

void ledInit(void){

	SIM->SCGC5 |= 0x400; // enable clock to port B
	PORTB->PCR[19] = 0x100; // GREEN LED AS miscelaneus
	PORTB->PCR[18] = 0x100; // RED LED AS miscelaneus
	PTB->PDDR |= 0xC0000; // Make leds as ouput pins
	PTB->PDOR |= 0xC0000; // turn them off

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
	for (i = 0; i < 3; i++) {
		PTB->PDOR &= ~0x80000; // turn on green led
		delayMs(500);
		PTB->PDOR |= 0x80000; // turn off red led
		delayMs(500);
	}
	PORTA->ISFR = 0x00000006; // clear interruption
}





// Time functions
void delayMs(int n) {
	int i;
	int j;
	for(i = 0 ; i < n; i++)
	for(j = 0 ; j < 7000; j++) { }
}

