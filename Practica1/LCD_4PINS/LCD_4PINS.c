// questo programa fa lo stesso che faceva l´ altro ma invece di usare 8 pins di dati ne prende solamente 4 piu quei di controllo

#include <MKL25Z4.h>
#include <String.h>



#define RS 1 /* BIT0 mask */
#define RW 2 /* BIT1 mask */
#define EN 4 /* BIT2 mask */

void delayMs(int n);
void delayUs(int n);
void LCD_nibble_write(unsigned char data, unsigned char control);
void LCD_command(unsigned char command);
void LCD_data(unsigned char data);
void LCD_init(void);


void LCD_init(void)
{
SIM->SCGC5 |= 0x1000; 

PORTD->PCR[0] = 0x100; 
PORTD->PCR[1] = 0x100; 
PORTD->PCR[2] = 0x100; 
PORTD->PCR[4] = 0x100; 
PORTD->PCR[5] = 0x100; 
PORTD->PCR[6] = 0x100; 
PORTD->PCR[7] = 0x100; 

PTD->PDDR |= 0xF7; 

delayMs(30); 
delayMs(10);
delayMs(1);

delayMs(1);
LCD_nibble_write(0x20, 0); /* use 4-bit data mode */
delayMs(1);
LCD_command(0x28); /* set 4-bit D, 2-line, 5x7 font */
LCD_command(0x06); /* move cursor right */
LCD_command(0x01); /* clr screen, move cursor home */
LCD_command(0x0F); /* turn on display, cursor blink*/}



void LCD_nibble_write(unsigned char data, unsigned char control)
{
data &= 0xF0; /* clear lower nibble for control */

control &= 0x0F; /* clear upper nibble for data */

PTD->PDOR = data | control; /* RS = 0, R/W = 0 */
PTD->PDOR = data | control | EN; /* pulse E */

delayMs(0);
PTD->PDOR = data;
PTD->PDOR = 0;
}





void LCD_command(unsigned char command)
{
/* upper nibble first */

LCD_nibble_write(command & 0xF0, 0); 

/* then lower nibble */

LCD_nibble_write(command << 4, 0); 

if (command < 4)

delayMs(4); /* commands 1 and 2 need up to 1.64ms */
else
delayMs(1); 
}




void LCD_data(unsigned char data)
{/* upper nibble first */
LCD_nibble_write(data & 0xF0, RS);
/* then lower nibble */ 
LCD_nibble_write(data << 4, RS); 
delayMs(1);
}

void delayMs(int n) {
int i;
int j;
for(i = 0 ; i < n; i++)
for(j = 0 ; j < 7000; j++) { }
}




int main(void)
{
LCD_init();
for(;;)
	{
	LCD_command(1); 
	delayMs(500);
	LCD_command(0x80);
		
		

	
	LCD_data('H'); 
	LCD_data('e');
	LCD_data('l');
	LCD_data('l');
	LCD_data('o');		
	LCD_data(' '); 
	LCD_data('W');
	LCD_data('o');
	LCD_data('r');
	LCD_data('l');
	LCD_data('d');
	LCD_data('!');
	LCD_command(0xC0);
	LCD_data(';');
	LCD_data('D');
	delayMs(1000);
	}
}
