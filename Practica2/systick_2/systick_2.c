// In this second program we must use the systick timer to toggle a green led at every  1000 ms delay.
// GREEN LED -> PTB19
#include <MKL25Z4.h>

// Will use the same function with some modifications

// This enables the pin 18 of port b as a miscellaneous output pin using PCR and PDDR Registers
void portInit(void);

void portInit(void){
	SIM->SCGC5 |= 0x400; // ENABLE CLOCK TO PORT B
	PORTB->PCR[19] = 0x100; // Make a miscellaneous pin (GREEN LED -> PTB19)
	PTB->PDDR |= 0x80000;  // MAKE THE PORT 19 AN OUTPUT
}




// Tune timer parameters
void timerInit(void);

void timerInit(void){
	SysTick->LOAD = 41949-1; // Set the counter to the necesary frequency so that the count last 1 ms
	SysTick->CTRL = 5;  // Enable | Source == system clock | no interruptions
}



// will count n delays of 1 ms
void delayMs(int n);

void delayMs(int n){
	timerInit();// intialize stuff
	for(int i = 0; i < n; i++){ // count n 1 ms delays
		while((SysTick->CTRL & 0x10000)==0){ // bit masking to check if the 16th bit (count flag) is active
			// wait
		}
	} 
	SysTick->CTRL = 0; // stop the timer by setting the enable to zero
}


int main(void){
	portInit();
	while(1){
		delayMs(1000); // 1 second == 1000 ms
		PTB->PTOR = 0x80000; // every 1 second negate the current led´s logic status
	}
}