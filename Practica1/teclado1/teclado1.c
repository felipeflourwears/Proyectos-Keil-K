#include <MKL25Z4.h>

void delayUs(int n);
void keypad_init(void);
char keypad_kbhit(void);



void keypad_init(void)
{
SIM->SCGC5 |= 0x0800; 
PORTC->PCR[0] = 0x103; 
PORTC->PCR[1] = 0x103; 
PORTC->PCR[2] = 0x103; 
PORTC->PCR[3] = 0x103;
PORTC->PCR[4] = 0x103; 
PORTC->PCR[5] = 0x103; 
PORTC->PCR[6] = 0x103; 
PORTC->PCR[7] = 0x103; 
PTD->PDDR = 0x0F; 
}






char keypad_kbhit(void)
{
int col;
PTC->PDDR |= 0x0F; 
PTC->PCOR = 0x0F;
delayUs(2); 
col = PTC->PDIR & 0xF0;
PTC->PDDR = 0; 
if (col == 0xF0)
return 0; 
else
return 1; 
}



void delayUs(int n) {
int i;
int j;
for(i = 0 ; i < n; i++)
for (j = 0; j < 8000; j++) {}
}


int main(void)
{
keypad_init();

SIM->SCGC5 |= 0x1000; /* enable clock to Port D */
PORTD->PCR[1] = 0x100; /* make PTD1 pin as GPIO */
PTD->PDDR |= 0x02; /* make PTD1 as output pin */
while(1)
{ if ( keypad_kbhit() != 0) /* if a key is pressed? */

PTD->PCOR |= 0x02; /* turn on blue LED */
else
PTD->PSOR |= 0x02; /* turn off blue LED */
}
}





