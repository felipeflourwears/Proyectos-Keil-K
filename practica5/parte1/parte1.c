
#include <MKL25Z4.h>
#include <String.h> // supports string stuff
#include <stdio.h>
#include <stdlib.h>


#define RS 0x01 
#define RW 0x04 
#define EN 0x08	 






void converterInit(void);
void LED_set(int s);
void LED_init(void);




// delay
void delayMs(int n);


// LCD pertinent funcitons
void LCD_nibble_write(unsigned char data, unsigned char control);
void LCD_command(unsigned char command);
void LCD_data(unsigned char data);
void LCD_init(void);





void converterInit(void)
{
	SIM->SCGC6 |= 0x8000000; // bit 27 to enable clock
	ADC0->SC2 &= ~0x40; // 10111111 for soft trigger
	ADC0->CFG1 = 0x40 | 0x10 | 0x04 | 0x00; // enable clock & 12 bit divider
}






// funcitons 
void writeTemperature(int temp){
	char str[1];
	int i;
	int len;
	sprintf(str,"%d",temp);
	len = strlen(str);
	LCD_command(1); 
	delayMs(500);
	LCD_command(0x80);
	LCD_data('T'); 
	LCD_data('e');
	LCD_data('m');
	LCD_data('p');
	LCD_data('e');
	LCD_data('r'); 
	LCD_data('a');
	LCD_data('t');
	LCD_data('u');
	LCD_data('r');
	LCD_data('e'); 
	LCD_data(':');
	LCD_command(0xC0);
	LCD_data(' ');		
	for(i = 0; i < len ; i ++){
		LCD_data(str[i]);
	}
	delayMs(1000);
	LCD_command(0x01);
 }

 
 
void delayMs(int n) {
	int i;
	int j;
	for(i = 0 ; i < n; i++)
	for(j = 0 ; j < 7000; j++) { }
}



void LCD_init(void)
{
SIM->SCGC5 |= 0x1000; // enable clock to port D
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




















int main (void)
{
	short int temp;
	LCD_init();
	converterInit(); 
	while (1) {
	ADC0->SC1[0] = 26; // conversion con sensor´s channel
		while(!(ADC0->SC1[0] & 0x80)) { 
		 // wait for conversion to finish
		} 
		temp = ADC0->R[0]; // obtain temp lecture | clear conversion flag
		writeTemperature(temp);

	}
}






