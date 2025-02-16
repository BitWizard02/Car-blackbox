/* 
 * File:   uart.h
 * Author: VEERESH
 *
 * Created on 7 January, 2025, 9:04 AM
 */


#define RX_PIN					TRISC7
#define TX_PIN					TRISC6

void init_uart(void);
void putch(unsigned char byte);
int puts(const char *s);
unsigned char getch(void);
unsigned char getch_with_timeout(unsigned short max_time);
unsigned char getche(void);






