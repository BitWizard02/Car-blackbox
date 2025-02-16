#include <xc.h>
#ifndef EEPROM_H
#define EEPROM_H

//for eeprom
unsigned char read_internal_eeprom(unsigned char address);
void write_internal_eeprom(unsigned char address, unsigned char ); 


/* Enum for maintaining the app state */
typedef enum {
    e_dashboard, e_main_menu, e_view_log, e_set_time, e_download_log, e_clear_log
} State_t;


extern State_t state; // App state

//Function declarations

//Dashboard function declaration
void view_dashboard(void);

//Storing events function declaration
void event_store(void);

//Password function declaration
//void password(void);

//main menu function declaration
void display_main_menu(void);

//View log function declaration
void view_log(void);

//Reading events function declaration
void event_reader(void);

//Change password function declaration
//void change_password(void);

//Set time function declaration
void set_time(void);

//Download log function _declaration
void download_log(void);

//Clear log function declaration
void clear_log(void);

//to store in eeprom store
void eeprom_store(void);

//for adc
void init_adc();
unsigned short read_adc();
void init_uart();

//for time from external eeprom
unsigned char calender_reg[4];
unsigned char time[9];
static void get_time(void);
void display_time(void);
#endif