//
// Created by mario on 30/11/22.
//
#include "MKL46Z4.h"
#include "lcd.h"
#include <stdbool.h>
#include "functions.h"
void irclk_ini()
{
    MCG->C1 = MCG_C1_IRCLKEN(1) | MCG_C1_IREFSTEN(1);
    MCG->C2 = MCG_C2_IRCS(0); //0 32KHZ internal reference clock; 1= 4MHz irc
}

void delay(void)
{
    volatile int i;

    for (i = 0; i < 500000; i++);
}

// RIGHT_SWITCH (SW1) = PTC3
void sw1_ini()
{
    SIM->COPC = 0;
    SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
    PORTC->PCR[3] |= PORT_PCR_MUX(1) | PORT_PCR_PE(1) | PORT_PCR_IRQC(0xA);
    GPIOC->PDDR &= ~(1 << 3);
    NVIC_SetPriority(31, 0); //prioridad max
    NVIC_EnableIRQ(31); //habilitamos la interrupcion
}

// LEFT_SWITCH (SW2) = PTC12
void sw2_ini()
{
    SIM->COPC = 0;
    SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
    PORTC->PCR[12] |= PORT_PCR_MUX(1) | PORT_PCR_PE(1) | PORT_PCR_IRQC(0xA);
    GPIOC->PDDR &= ~(1 << 12);
    NVIC_SetPriority(31, 0); //prioridad max
    NVIC_EnableIRQ(31); //habilitamos la interrupcion
}

int sw1_check()
{
    return( !(GPIOC->PDIR & (1 << 3)) );
}

int sw2_check()
{
    return( !(GPIOC->PDIR & (1 << 12)) );
}

// RIGHT_SWITCH (SW1) = PTC3
// LEFT_SWITCH (SW2) = PTC12
void sws_ini()
{
    SIM->COPC = 0;
    SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
    PORTC->PCR[3] |= PORT_PCR_MUX(1) | PORT_PCR_PE(1) | PORT_PCR_IRQC(0xA) ;
    PORTC->PCR[12] |= PORT_PCR_MUX(1) | PORT_PCR_PE(1) | PORT_PCR_IRQC(0xA) ;
    GPIOC->PDDR &= ~(1 << 3 | 1 << 12);

    NVIC_SetPriority(31, 0); //prioridad max
    NVIC_EnableIRQ(31); //habilitamos la interrupcion
}

// LED_GREEN = PTD5
void led_green_ini()
{
    SIM->COPC = 0;
    SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
    PORTD->PCR[5] = PORT_PCR_MUX(1);
    GPIOD->PDDR |= (1 << 5);
    GPIOD->PSOR = (1 << 5);
}

void led_green_toggle()
{
    GPIOD->PTOR = (1 << 5);
}

void led_green_clear()
{
    GPIOD->PSOR = (1 << 5);
}

void led_green_set()
{
    GPIOD->PCOR = (1 << 5);
}


// LED_RED = PTE29
void led_red_ini()
{
    SIM->COPC = 0;
    SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
    PORTE->PCR[29] = PORT_PCR_MUX(1);
    GPIOE->PDDR |= (1 << 29);
    GPIOE->PSOR = (1 << 29);
}

void led_red_toggle(void)
{
    GPIOE->PTOR = (1 << 29);
}
void led_red_clear(void)
{
    GPIOE->PSOR = (1 << 29);
}

void led_red_set(void)
{
    GPIOE->PCOR = (1 << 29);
}


// LED_RED = PTE29
// LED_GREEN = PTD5
void leds_ini()
{
    SIM->COPC = 0;
    SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK | SIM_SCGC5_PORTE_MASK;
    PORTD->PCR[5] = PORT_PCR_MUX(1);
    PORTE->PCR[29] = PORT_PCR_MUX(1);
    GPIOD->PDDR |= (1 << 5);
    GPIOE->PDDR |= (1 << 29);
    // both LEDS off after init
    GPIOD->PSOR = (1 << 5);
    GPIOE->PSOR = (1 << 29);
}
