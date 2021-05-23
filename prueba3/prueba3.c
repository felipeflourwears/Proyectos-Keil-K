// Blink a led in a protoboard with PTC12 port
#include <MKL25Z4.H>

void delay(int n) {
 int i;
 for(i = 0; i < n; i++) ;
}


int main(){
	SIM_SCGC5 |= 0x400;
	PORTC_PCR12 |= 0x100; // set portc 12 as output
	GPIOC_PDDR |= 0x1000;
	
	
	while(1){
		GPIOC_PDOR &= ~0x1000; 
		delay(500);
		GPIOC_PDOR |= 0x1000; 
		delay(500);	
	}
}



