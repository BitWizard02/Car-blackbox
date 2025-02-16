/*
 * File:   functions.c
 * Author: VEERESH
 *
 * Created on 30 December, 2024, 8:59 AM
 */


#include <xc.h>
#include "black_box.h"
#include"clcd.h"
#include"keypad.h"
#include "uart.h"
#include <string.h>
#include "ds1307.h"
#include "i2c.h"

#define _XTAL_FREQ 20000000
//char time[] = "00:00:00";
char gear[9][3] = {"ON","GN", "G1", "G2", "G3", "G4", "G5", "GR", " C"};
static int gearflag = 0;
unsigned int speed;
extern unsigned char key;
static int menuflag = 0, starflag = 0;
char menu[4][17] = {"view_log    ", "clear_log   ", "set_time    ", "download_log     "};
char storedata[10];
static int entryflag = 2;
static int address = 0x00;
char data[10];
char dataarr[18];
int event_count = 0;
static int display_count = 0;
int wait = 0;
unsigned char clock_reg[3];
int timewhere, time_flag = 0,flag=0,time_delay = 0;
int hour,min,sec;
char ptr[10][16];
int add_count=0;
int view_index = 0;

void display_time(void) {
    if (timewhere == 0) {
        clcd_print(time, LINE2(0));
    } else {
        clcd_print(time, LINE2(4));
    }

    //	if (clock_reg[0] & 0x40)
    //	{
    //		if (clock_reg[0] & 0x20)
    //		{
    //			clcd_print("PM", LINE2(12));
    //		}
    //		else
    //		{
    //			clcd_print("AM", LINE2(12));
    //		}
    //	}
}

static void get_time(void) {
    clock_reg[0] = read_ds1307(HOUR_ADDR);
    clock_reg[1] = read_ds1307(MIN_ADDR);
    clock_reg[2] = read_ds1307(SEC_ADDR);

    if (clock_reg[0] & 0x40)//12 HR FORMAT
    {
        time[0] = '0' + ((clock_reg[0] >> 4) & 0x01);
        time[1] = '0' + (clock_reg[0] & 0x0F);
    } else//24 HR FORMAT
    {
        time[0] = '0' + ((clock_reg[0] >> 4) & 0x03);
        time[1] = '0' + (clock_reg[0] & 0x0F);
    }
    time[2] = ':';
    time[3] = '0' + ((clock_reg[1] >> 4) & 0x0F);
    time[4] = '0' + (clock_reg[1] & 0x0F);
    time[5] = ':';
    time[6] = '0' + ((clock_reg[2] >> 4) & 0x0F);
    time[7] = '0' + (clock_reg[2] & 0x0F);
    time[8] = '\0';
}

void eeprom_store(void) {

    storedata[0] = time[0];
    storedata[1] = time[1];
    storedata[2] = time[3];
    storedata[3] = time[4];
    storedata[4] = time[6];
    storedata[5] = time[7];
    storedata[6] = gear[gearflag][0];
    storedata[7] = gear[gearflag][1];
    storedata[8] = (speed / 10); //tens place of speed
    storedata[9] = (speed % 10); // units/once place of speed value

    //for swap the values 
    if (address == 100) {
        
        event_count=9;
                address = 90;
                for (int j = 0; j < 90; j++) {
                    write_internal_eeprom(j, read_internal_eeprom(j + 10));
                }
//                i--;
            }
//    if (event_count > 0 && event_count < 10) {

        for (int i = 0; i < 10; i++) {
                write_internal_eeprom(address++, storedata[i]);
        }
        event_count++;

//    } else {
//        for (int i = event_count - 10; i < event_count; i++) {
//            if (address == 0x99) {
//                address = 0x00;
//                for (int j = 0; j < 9; j++) {
//                    write_internal_eeprom(address++, storedata[(i - (event_count - 10)) + 1]);
//                }
//                i--;
//            }
//            // Write data to the current address
//            write_internal_eeprom(address++, storedata[(i - (event_count - 10))]);
//        }
//    }
}

void view_dashboard(void) {
    //CLEAR_DISP_SCREEN;
    clcd_print("TIME", LINE1(0));
    clcd_print("EV", LINE1(10));
    clcd_print("SP", LINE1(14));
    //    clcd_print(time, LINE2(0));
    timewhere = 0;
    get_time();
    display_time();

    if (key == 1) {
        if (gearflag == 7) {
            gearflag = 7;
        } else if (gearflag == 8) {
            gearflag = 0;
        } else {
            gearflag++;
            eeprom_store();
            view_index=1;
        }

    } else if (key == 2) {
        if (gearflag <= 1) {
            gearflag = 1;
        } else if (gearflag == 8) {
            gearflag = 0;
        } else {
            gearflag--;
            eeprom_store();
            view_index=1;
        }

    }
    clcd_print(gear[gearflag], LINE2(10));
    speed = read_adc();
    speed = speed / 10.33;
    clcd_putch((speed / 10) + 48, LINE2(14));
    clcd_putch((speed % 10) + 48, LINE2(15));

    if (key == 3) {
        gearflag = 8;

    }
    //eeprom_store();
    if (key == 11) {
        CLEAR_DISP_SCREEN;
        state = e_main_menu;

    }


}

void display_main_menu(void) {
    
    clcd_print(menu[menuflag], LINE1(2));
    clcd_print(menu[menuflag + 1], LINE2(2));
    if (starflag == 0) {
        clcd_putch('*', LINE1(0));

    } else if (starflag == 1) {
        clcd_putch(' ', LINE1(0));
        clcd_putch('*', LINE2(0));
    }
    if (key == 2 && starflag == 0) {
        clcd_putch(' ', LINE1(0));
        clcd_putch('*', LINE2(0));
        starflag = 1;
        entryflag++;
    } else if (key == 2) {
        if (menuflag < 2) {
            menuflag++;
            entryflag++;
        }
    } else if (key == 1 && starflag == 1) {
        clcd_putch('*', LINE1(0));
        clcd_putch(' ', LINE2(0));
        starflag = 0;
        entryflag--;
    } else if (key == 1) {
        if (menuflag > 0) {
            menuflag--;
            entryflag--;
        }
    }
    if (key == 11 && entryflag == 2) {
        //CLEAR_DISP_SCREEN;
        state = e_view_log;
    } else if (key == 11 && entryflag == 3) {
        //CLEAR_DISP_SCREEN;
        state = e_clear_log;
    } else if (key == 11 && entryflag == 4) {
        //CLEAR_DISP_SCREEN;
        state = e_set_time;
    } else if (key == 11 && entryflag == 5) {
        //CLEAR_DISP_SCREEN;
        state = e_download_log;
    }


    //back to dashboard
    if (key == 12) {
        CLEAR_DISP_SCREEN;
        state = e_dashboard;
    }

}

void view_log(void) {
    if (event_count == 0) {

        clcd_print("NO Log Available", LINE1(0));
        clcd_print("                 ", LINE2(0));

    } else {
        clcd_putch('#', LINE1(0));
        clcd_print("TIME    ", LINE1(2));
        clcd_print("EV ", LINE1(11));
        clcd_print("SP  ", LINE1(14));
        static int x = 0;
        if (key == 1) {
            if (x < (event_count - 1))
                x++;
        } else if (key == 2) {
            if (x > 0)
                x--;
        }
        for (int k = x * 10; k < ((x * 10) + 10); k++) {
            data[k - (x * 10)] = read_internal_eeprom(0x00 + k);
        }
        clcd_putch((x) + 48, LINE2(0));
        clcd_putch(data[0], LINE2(2));
        clcd_putch(data[1], LINE2(3));
        clcd_putch(':', LINE2(4));
        clcd_putch(data[2], LINE2(5));
        clcd_putch(data[3], LINE2(6));
        clcd_putch(':', LINE2(7));
        clcd_putch(data[4], LINE2(8));
        clcd_putch(data[5], LINE2(9));
        clcd_putch(' ', LINE2(10)); 
        clcd_putch(data[6], LINE2(11));
        clcd_putch(data[7], LINE2(12));
        clcd_putch(' ', LINE2(13));
        clcd_putch(data[8] + '0', LINE2(14));
        clcd_putch(data[9] + '0', LINE2(15));
    }
    if (key == 12) {
        CLEAR_DISP_SCREEN;
        state = e_main_menu;

    }

}

void download_log(void) {
    // CLEAR_DISP_SCREEN;
    clcd_print("downloaded     ", LINE1(0));
    clcd_print("SUCCESSFULL    ", LINE2(0));
    static int y = 0;

//    if (y == 0) {
//        puts("TIME      GR   SP\n\r");
//        if (wait++ >= 3000) {
//            display_count = y = 0;
//            CLEAR_DISP_SCREEN;
//            state = e_main_menu;
//            wait = 0;
//        }
//
//    }


    //  putch((x + 1) + 48);
//    display_count++;
//    if (display_count <= event_count) {
//        y++;
//    }

    if(wait==0)
    {
     puts("TIME      GR   SP\n\r");
     
    for(y=0;y<event_count;y++)
    {
        
    
    for (int j = y * 10; j < ((y * 10) + 10); j++) {

        dataarr[j - (y * 10)] = (read_internal_eeprom(0x00 + j));

    }

    if (display_count <= event_count) {
        putch((dataarr[0]));
        putch((dataarr[1]));
        putch(':');
        putch((dataarr[2]));
        putch((dataarr[3]));
        putch(':');
        putch(dataarr[4]);
        putch(dataarr[5]);
        putch(' ');
        putch(dataarr[6]);
        putch(dataarr[7]);
        putch(' ');
        putch((dataarr[8] + '0'));
        putch((dataarr[9] + '0'));
        puts("\n\r");
    }
    }
    }
    if (wait++ >= 3000) {
        display_count = y = 0;
        CLEAR_DISP_SCREEN;
        state = e_main_menu;
        wait = 0;
    }
    
}

void clear_log(void) {
    event_count = 0;
    clcd_print("  clear_log    ", LINE1(0));
    clcd_print("  SUCCESSFULL   ", LINE2(0));
    if (key == 12) {
        CLEAR_DISP_SCREEN;
        state = e_main_menu;
    }
}



void set_time(void)
{
    clcd_print("HH:MM:SS", LINE1(2));
//    clcd_print("                ", LINE2(0));
    static int edit_flag = 0,field_flag,delay;
    if(edit_flag == 0)
    {
        edit_flag=1;
        hour = read_ds1307(HOUR_ADDR);
        hour=(hour>>4)*10+(hour& 0xF);
        min  = read_ds1307(MIN_ADDR);
        min = (min >> 4)*10 + (min & 0xF);
        sec = read_ds1307(SEC_ADDR);
        sec = (sec >> 4)*10 + (sec & 0xF);
    CLEAR_DISP_SCREEN;
    }
    
    if(delay++ < 1000)
    {
    clcd_putch((hour/10) + 48, LINE2(2));
    clcd_putch((hour%10) + 48, LINE2(3));
    clcd_print(":",LINE2(4));
    clcd_putch((min/10) + 48, LINE2(5));
    clcd_putch((min%10) + 48, LINE2(6));
    clcd_print(":",LINE2(7));
    clcd_putch((sec/10) + 48, LINE2(8));
    clcd_putch((sec%10) + 48, LINE2(9));
    }
    else if(delay<2000)
    {
        if(field_flag == 1)
        {
            clcd_print("  ",LINE2(2));
           
        }
        else if(field_flag == 2)
        {
            clcd_print("  ",LINE2(5));
                    }
        else 
        {
            clcd_print("  ",LINE2(8));
                    }
    }
    else
    {
        delay = 0;
    }
    
    if(key == 1 )
    {
        if(field_flag == 1)
        {
            if(hour++ == 23)
                hour = 0;
        }
        else if(field_flag == 2)
        {
            if(min++ == 59)
                min = 0;
        }
        else
        {
            if(sec++ == 59)
                sec = 0;
        }
            
    }
    
    if(key == 2)
    {
        if(field_flag++ == 2)
            field_flag = 0;
    }
    
    if(key == 11)
    {
        CLEAR_DISP_SCREEN;
        clcd_print("set time",LINE1(0));
        clcd_print("Successfully",LINE2(3));

        for(unsigned long k = 500000;k--;);

        write_ds1307(HOUR_ADDR, (hour/10) << 4 | (hour % 10));
        write_ds1307(MIN_ADDR, (min/10) << 4 | (min % 10));
        write_ds1307(SEC_ADDR, (sec/10) << 4 | (sec % 10));
        CLEAR_DISP_SCREEN;

        state = e_main_menu;
        edit_flag = 0;
    }
    else if(key == 12)
    {
        state = e_main_menu;
        edit_flag = 0;
    }
}