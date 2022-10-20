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
    SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK; //habilitamos puerto C
    //establecemos la interrupcion on fallin edge (1010)
    PORTC->PCR[3] |= PORT_PCR_MUX(1) | PORT_PCR_PE(1) | PORT_PCR_IRQC(0xA) ;
    GPIOC->PDDR &= ~(1 << 3); //lo establecemos como entrada (0)

    NVIC_SetPriority(31, 0); //prioridad max
    NVIC_EnableIRQ(31); //habilitamos la interrupcion

}

int sw1_check(){
    return(!(GPIOC->PDIR & (1<<3)));
}


// BOTON_2 = PTC12
void button2_init(){

    SIM->COPC = 0;
    SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK; //habilitamos puerto C
    //establecemos la interrupcion on fallin edge (1010)
    PORTC->PCR[12] |= PORT_PCR_MUX(1) | PORT_PCR_PE(1) | PORT_PCR_IRQC(0xA) ;
    GPIOC->PDDR &= ~(1 << 12); //lo establecemos como entrada (0)

    NVIC_SetPriority(31, 0); //prioridad max
    NVIC_EnableIRQ(31); //habilitamos la interrupcion


}

int sw2_check(){
    return(!(GPIOC->PDIR & (1<<12)));
}



void PORTDIntHandler(void) {

    PORTC->PCR[3] |=PORT_PCR_ISF(1);
    PORTC->PCR[12] |=PORT_PCR_ISF(1);
    switch(estado){
        case 00: //ninguna abierta

            if(sw1_check()){
                //cambiamos al estado 01 y se enciende el led correspondinte (rojo)
                estado=01;
                led_green_clear();
                led_red_set();
            }else if(sw2_check()){
                //cambiamos al estado 10 y se enciende el led correspondinte (rojo)
                estado=10;
                led_green_clear();
                led_red_set();
            }
            break;

        case 01: //abierta puerta 1

            if(sw1_check()){
                estado=00;
                led_green_set();
                led_red_clear();
            }else if(sw2_check()){
                estado=11;
                led_green_clear();
                led_red_set();
            }

            break;

        case 10: //abierta puerta 2
            if(sw1_check()){
                estado=11;
                led_green_clear();
                led_red_set();
            }else if(sw2_check()){
                estado=00;
                led_green_set();
                led_red_clear();
            }

            break;

        case 11: //ambas puertas abiertas
            if(sw1_check()){
                estado=10;
                led_green_clear();
                led_red_set();
            }else if(sw2_check()){
                estado=01;
                led_green_clear();
                led_red_set();

            }
            break;
    }


    return;
}
int main(void)
{
    button1_init();
    button2_init();
    led_green_init();
    led_red_init();

    //iniciamos los leds como en el estaod 0
    estado=00;
    led_green_set();
    led_red_clear();
    while (1) {

    }

    return 0;
}