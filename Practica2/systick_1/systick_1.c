#include <MKL25Z4.h>

/*
This is my first program to get aquinted with the systick timer of KL25Z UWU
In a nutshell, it will initialize the STRELOAD to a certain value and will blink the red led at each timestep
*/


// This enables the pin 18 of port b as a miscellaneous output pin using PCR and PDDR Registers
void portInit(void);

void portInit(void){
	SIM->SCGC5 |= 0x400; // ENABLE CLOCK TO PORT B
	PORTB->PCR[18] = 0x100; // Make a miscellaneous pin
	PTB->PDDR |= 0x40000;  // MAKE THE PORT 19 AN OUTPUT
}

// Tune timer parameters
void timerInit(void);
void timerInit(void){
	SysTick->LOAD = 0xFFFFFF; // Set the counter to the maximum available number
	SysTick->CTRL = 5;  // Enable | Source == system clock | no interruptions
}

int main(void){
	int c;
	portInit();
	timerInit();
	while (1) {
	c = SysTick->VAL; // We assign the current count to a variable 
	PTB->PDOR = c >> 4;  // Syncronize the blinking with the counter displacement. 
			// Must be 4 because instruccions said so (in order for the blinking to be visible to human eyes). 

	}

}