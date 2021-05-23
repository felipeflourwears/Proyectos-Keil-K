#include <MKL25Z4.h>

/*
Count values
25% 10925
50% 21851
75% 32777
100% 43703

*/
void PWM_INIT(void);

void PWM_INIT(void){
	SIM->SCGC5 |= 0x1000; // Enable to port D
	PORTD->PCR[1] = 0x0400; // enable TMP0 to Pin 1
	SIM->SCGC6 |= 0x01000000; // enable clock to tpm 0
	SIM->SOPT2 |= 0x01000000; // 41.96 HZ clock

	TPM0->SC = 0; // enable timer
	TPM0->CONTROLS[1].CnSC = 0x20 | 0x08;  // enable timer (counter)
	TPM0->MOD = 43702; // MODULE
	TPM0->CONTROLS[1].CnV = 10925; // COUNTER (WIDTH)
	TPM0->SC = 0x0C;    // Enable with prescaler = 16
}

int main (void) {

	PWM_INIT();
	while (1) { } 
}


