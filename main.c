#include "MKL46Z4.h"

// LED (RG)
// LED_GREEN = PTD5
// LED_RED = PTE29
// BOTON_1 = PTC3
// BOTON_2 = PTC12

short estado;

void delay(void)
{
    volatile int i;

    for (i = 0; i < 1000000; i++);
}


// LED_GREEN = PTD5
void led_green_init()
{
    SIM->COPC = 0;
    SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
    PORTD->PCR[5] = PORT_PCR_MUX(1);
    GPIOD->PDDR |= (1 << 5);
    GPIOD->PSOR = (1 << 5);
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
void led_red_init()
{
    SIM->COPC = 0;
    SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
    PORTE->PCR[29] = PORT_PCR_MUX(1);
    GPIOE->PDDR |= (1 << 29);
    GPIOE->PSOR = (1 << 29);
}

void led_red_clear(void)
{
    GPIOE->PSOR = (1 << 29);
}

void led_red_set(void)
{
    GPIOE->PCOR = (1 << 29);
}


// BOTON_1 = PTC3
void button1_init(){
    SIM->COPC = 0;
    SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK; //habilitamos puerto B
    PORTC->PCR[3] |= PORT_PCR_MUX(1) | PORT_PCR_PE(1);
    GPIOC->PDDR &= ~(1 << 3); //lo establecemos como entrada (0)
}

int sw1_check(){
    return(!(GPIOC->PDIR & (1<<3)));
}


// BOTON_2 = PTC12
void button2_init(){
    SIM->COPC = 0;
    SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK; //habilitamos puerto B
    PORTC->PCR[12] |= PORT_PCR_MUX(1) | PORT_PCR_PE(1);
    GPIOC->PDDR &= ~(1 << 12); //lo establecemos como entrada (0)
}

int sw2_check(){
    return(!(GPIOC->PDIR & (1<<12)));
}


int main(void)
{
    button1_init();
    button2_init();
    led_green_init();
    led_red_init();
    estado=00;


    while (1) {

        switch(estado){
            case 00: //ninguna abierta
                led_green_set();
                led_red_clear();
                while (1){
                    if(sw1_check()){
                        estado=01;
                        while(sw1_check()){}//mientras siga pulsada
                        break;
                    }
                    if(sw2_check()){
                        estado=10;
                        while(sw2_check()){}
                        break;
                    }
                }


                break;
            case 01: //abierta puerta 1
                led_green_clear();
                led_red_set();
                while (1){
                    if(sw1_check()){
                        estado=00;
                        while(sw1_check()){}
                        break;
                    }
                    if(sw2_check()){
                        estado=11;
                        while(sw2_check()){}
                        break;
                    }
                }

                break;

            case 10: //abierta puerta 2
                led_green_clear();
                led_red_set();
                while (1){
                    if(sw1_check()){
                        estado=11;
                        while(sw1_check()){}
                        break;
                    }
                    if(sw2_check()){
                        estado=00;
                        while(sw2_check()){}
                        break;
                    }
                }
                break;

            case 11: //ambas puertas abiertas
                led_green_clear();
                led_red_set();
                while (1){
                    if(sw1_check()){
                        estado=10;
                        while(sw1_check()){}
                        break;
                    }
                    if(sw2_check()){
                        estado=01;
                        while(sw2_check()){}
                        break;
                    }
                }
                break;
        }

    }

    return 0;
}