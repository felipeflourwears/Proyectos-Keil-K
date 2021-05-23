#include <MKL25Z4.H>
int main (void) {
void delayMs(int n);

SIM->SCGC5 |= 0x400; 
PORTA->PCR[1] = 0x100; 
PTA->PDDR |= 0x1; 

while (1) {
PTA->PDOR &= ~0x02; 
PTA->PDOR |= 0x02; 
delayMs(500);
}}


void delayMs(int n) {
int i;
int j;
for(i = 0 ; i < n; i++)
for (j = 0; j < 7000; j++) {}
}
