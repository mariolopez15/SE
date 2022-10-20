#include "MKL46Z4.h"

// LED (RG)
// LED_GREEN = PTD5
// LED_RED = PTE29
// Boton = PTC3

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

void led_green_toggle()
{
  GPIOD->PTOR = (1 << 5);
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

void led_red_toggle(void)
{
  GPIOE->PTOR = (1 << 29);
}

void button_init(){

    SIM->COPC = 0;
    SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK; //habilitamos puerto C
    //establecemos la interrupcion on fallin edge (1010)
    PORTC->PCR[3] |= PORT_PCR_MUX(1) | PORT_PCR_PE(1) | PORT_PCR_IRQC(0xA) ;
    GPIOC->PDDR &= ~(1 << 3); //lo establecemos como entrada (0)

    NVIC_SetPriority(31, 0); //prioridad max
    NVIC_EnableIRQ(31); //habilitamos la interrupcion



}

void PORTDIntHandler(void){

    PORTC->PCR[3] |=PORT_PCR_ISF(1); //limpiamos la interrupcion
    led_green_toggle();
    led_red_toggle();
}


int main(void)
{
    button_init();
    led_green_init();
    led_red_init();


    while (1) {
        //bucle esperando que se produzca la interrupcion
    }


    return 0;
}
