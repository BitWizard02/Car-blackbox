/*
 * File:   main.c
 * Author: VERESH
 *
 * Created on 30 DECEMBER, 2024, 8:55 AM
 */
#include <xc.h>
#include <string.h>
#include "black_box.h"
#include"clcd.h"
#include"keypad.h"
#include "uart.h"
#include "ds1307.h"
#include "i2c.h"
unsigned char key;


State_t state;

void init_config()
{
    init_clcd();
    init_adc();
    init_uart();
    init_matrix_keypad();
    init_i2c();
	init_ds1307();
    state = e_dashboard;
    
}

void main(void) 
{
    init_config();
    
    while(1)
    {
        // Detect key press
        key=read_switches(1);
        
        switch (state)
        {
            case e_dashboard:
                // Display dashboard
                view_dashboard();
                break;
            
            case e_main_menu:
                // Display dashboard
                display_main_menu();
                break;
            
            case e_view_log:
                // Display dashboard
                view_log();
                break;
                 
            case e_download_log:
                download_log();
                break;
                
            case e_clear_log:
                clear_log();
                break;
                
                      
            case e_set_time:
                set_time();
                break;
                
        }
        
    }
    
}
