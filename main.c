#include "MKL46Z4.h"

// LED (RG)
// LED_GREEN = PTD5
// LED_RED = PTE29

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
    SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK; //habilitamos puerto B
    PORTC->PCR[3] |= PORT_PCR_MUX(1) | PORT_PCR_PE(1);
    GPIOC->PDDR &= ~(1 << 3); //lo establecemos como entrada (0)

}

int sw1_check(){
    return(!(GPIOC->PDIR & (1<<3)));
}

int main(void)
{
    button_init();
  led_green_init();

  while (1) {

      if(sw1_check()){
          led_green_toggle();
          while(sw1_check()){}
      }


    //delay();
  }

  return 0;
}
