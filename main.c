#include "MKL46Z4.h"
#include "lcd.h"
#include <stdbool.h>

// LED (RG)
// LED_GREEN = PTD5 (pin 98)
// LED_RED = PTE29 (pin 26)

// SWICHES
// RIGHT (SW1) = PTC3 (pin 73)
// LEFT (SW2) = PTC12 (pin 88)

// Enable IRCLK (Internal Reference Clock)
// see Chapter 24 in MCU doc

bool pushed; //0-> sw1 1->sw2
bool done; //ya se ha establecido
uint8_t min;
uint8_t seg;


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


void PORTDIntHandler(void) {
    //Rutina de servicio de los botones
    /*
    PORTC->PCR[3] |=PORT_PCR_ISF(1);
    PORTC->PCR[12] |=PORT_PCR_ISF(1);
    if(!done){ //en caso de no haber sido contabiizado
        if(led==0){
            if(sw1_check()){
                hits++;
            }else{
                misses++;
            }
        }else{
            if(sw2_check()){
                hits++;
            }else{
                misses++;
            }
        }
        done=true;
    }
    */

    PORTC->PCR[3] |=PORT_PCR_ISF(1);
    PORTC->PCR[12] |=PORT_PCR_ISF(1);
    if(sw1_check() && sw2_check()){
        //Si estan pulsados los dos se establece el tiempo
        if(pushed==0){
            /* Si el ultimo en ser pulsado fue el sw1(pushed == 0) quiere decir que se
             * pulso primero el sw2 y por lo tanto esa pulsacion, al no estar aun los
             * dos pulsados a la vez, se contó. Por eso ahora se descuenta */

            if(seg==0){
                //el anterior a 0 es 59
                seg=59;
            }else{
                seg--;
            }

        }else{
            if(min==0){
                min=59;
            }else{
                min--;
            }
        }

        done=true;

    }else{
        //Cuando solo esta pulsado uno en un instante se actualiza el tiempo
        if(sw1_check()){
            //si esta pulsado se suma un segundo al tiempo
            pushed=0;
            seg++;
            if(seg==60){
                seg=0;
            }
        }else if(sw2_check()){
            //Si esta pulsaodo se suma un minuto al tiempo
            pushed=1;
            min++;
            if(min==60){
                min=0;
            }
        }
    }


}
// Hit condition: (else, it is a miss)
// - Left switch matches red light
// - Right switch matches green light

int main(void)
{
  irclk_ini(); // Enable internal ref clk to use by LCD

  led_green_ini();
  led_red_ini();
  sws_ini();
  lcd_ini();
  //inicializamos el marcador
  min=55;
  seg=55;
  done=false;
  lcd_display_time(min, seg);


  //primero en un bucle se espera a que se establezca el tiempo, cuando este establecido se, la interrupcion por botones se deshabilita
  //despues se llama al timer para que vaya disminuyendo el tiempo

  while(!done){
      lcd_display_time(min, seg);
      delay();
      delay();
      lcd_ini();//clear
      delay();
  }
  NVIC_DisableIRQ(31); //una vez establecida se deshabilita la interrupcion por botones
  lcd_display_time(min, seg);

  while(1){}
  /*
  while (index < 32) {
    done=false;
    if (sequence & (1 << index)) { //odd

      // Switch on green led
      led=0;//led verde encencido
      led_green_set();
      led_red_clear();
    } else { //even

      // Switch on red led
      led=1; //led rojo encendido
      led_green_clear();
      led_red_set();
    }

    delay(); //tiempo para presionar
    if(!done){ //en caso de no haber pulsado se cuenta como miss
        misses++;
        done=true;//como ya ha sido contabilizado se pone a true
    }
    led_red_clear();
    led_green_clear();
    lcd_display_time(hits, misses);
    delay();
    index++;

  }

  // Stop game and show blinking final result in LCD: hits:misses
    led_red_clear();
    led_green_clear();
    NVIC_DisableIRQ(31);

  while (1) {
      lcd_display_time(hits, misses);
      delay();
      lcd_ini();//clear
      delay();
  }
   */

  return 0;
}
