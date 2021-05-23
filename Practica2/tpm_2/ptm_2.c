// In this program, we will use the timer tmp1 to toggle the blue led of the kl25z at its maximum delay available





// In this program, we will use the timer tmp1 to toggle the blue led of the kl25z at its maximum delay available

#include <MKL25Z4.H>
// BLUE ->  PTD1
// This function will tune the parameters for the blue led

void ledInit(void);

void ledInit(void){
	SIM->SCGC5 |= 0x1000;  // enable clock to port D
	PORTD->PCR[1] = 0x100;  // make led pin a miscellaneous pin
	PTD->PDDR |= 0x02; // make blue led pin an output gpio
}


void timerInit(void){
	SIM->SCGC6 |= 0x02000000; //enable the clock 6 to tpm1
	SIM->SOPT2 |= 0x01000000;  // Enable 41.94 hz to clock
	TPM1->SC = 0; // disbale timer
	TPM1->SC = 0x02; 
	//TPM1->MOD =  0xFFFF; // SET MAX VALUE 65536
	TPM1->MOD =  40960-1; // SET MAX VALUE 65536
	TPM1->SC |= 0x80; // CREAL TOF
	TPM1->SC |= 0x08;   // FRERRUNNING MODE
}

int main (void) {
	ledInit();
	timerInit();

		while (1) {
			while((TPM1->SC & 0x80) == 0) { } 
				TPM1->SC |= 0x80; 
				PTD->PTOR = 0x02; 

		}

}




/*
void timerInit(void){
	SIM->SCGC6 |= 0x02000000; // asign clock to tpm1
	SIM->SOPT2 |= 0x01000000; // assign 32kh clock

	TPM1->SC = 0; // disble
	TPM1->SC = 0x02;  // 4
	TPM1->MOD = 40960-1; // load
	TPM1->SC |= 0x80; // clar flag
	TPM1->SC |= 0x08;

}

*/




