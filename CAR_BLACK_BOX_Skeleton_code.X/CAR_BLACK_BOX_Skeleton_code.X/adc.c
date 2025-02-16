/*
 * File:   adc.c
 * Author: VEERESH
 *
 * Created on 31 December, 2024, 8:13 AM
 */

#include <xc.h>
#include "black_box.h"
#include"clcd.h"
#include"keypad.h"

void init_adc()
{
    CHS3=0;//CHANNEL 4
    CHS2=1;
    CHS1=0;
    CHS0=0;
    
    GO=0;//NO CONVERSION
    ADON=1;//ADC ENABLED
    
    VCFG1=0;//NO REAL VOLTAGE
    VCFG0=0;
    
    PCFG3=1;//MAKE AN4 AS ANALOG CHANNEL
    PCFG2=0;
    PCFG1=1;
    PCFG0=0;
    
    ADFM=1;//RIGHT JUSTIFIED
    
    ADCS2=0;//0.625 MHZ OF ADC CONVERSION CLOCK
    ADCS1=1;
    ADCS0=0;
    
    ACQT2=1;//8 TAD (12.6 MICRO SEC ACQ TIME)
    ACQT1=0;
    ACQT0=0;
}

unsigned short read_adc()
{
    //INITITATE CONVERSION
    GO=1;
    //WAIT TILL CONVERSION COMPLETE
    while(GO);
    //FETCH VALUE FROM ADC REG
    return ADRESL|(ADRESH<<8);
}
