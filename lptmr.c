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
bool counted;
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


void tpm_ini(){

    SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK; //iniacilizamos el puerto A que es donde esta el TPM0
    SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK; //activamos el reloj para el TPM
    SIM->SOPT2 |= SIM_SOPT2_TPMSRC(3); //trabajara con el MCGIRCLK que trabaja a 32 kHz
    TPM0->SC = 0; //deshabilitamos el tpm para modificar el PS
    TPM0->SC = (TPM_SC_PS(4)); //dividimos por 32
    //Como trabaja a 32Khz pero lo dividimos entre 32 pasara 1s al completar 1000 ciclos
    TPM0->MOD = TPM_MOD_MOD(2000); //El objetivo son 1000
    TPM0->SC |= TPM_SC_TOIE_MASK; //habilitamos que se produzca la interrupcion
    NVIC_SetPriority(17, 0); //TPM0_IRQn es el 17
    NVIC_ClearPendingIRQ(17);
    NVIC_EnableIRQ(17);
}

void tmp_start(){
    TPM0->SC |= TPM_SC_CMOD(1); //se establece el modo de conteo
}


void minus_time(){

    if(seg==0){
        if(min!=0){
            min--;
            seg=59;
        }else{
            min=0;
            seg=0;
            counted=true;
        }
    }else{
        seg--;
    }

}

void FTM0IntHandler() {
    PORTA->PCR[0] |=PORT_PCR_ISF(1);
    TPM0->SC |= TPM_SC_TOF_MASK; //al ser llamado la interrupcion pone el bit del overflow a 1 ya que el valor d ela cuenta alcanzo el objetivo
    minus_time();
    lcd_display_time(min, seg);

}


void PORTDIntHandler(void) {
    PORTC->PCR[3] |=PORT_PCR_ISF(1);
    PORTC->PCR[12] |=PORT_PCR_ISF(1);
    if(!done){
        //Cuando el tiempo aun no este establecido, es decir cuando se pulsen los botones para establecer el valor del contador
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

    }else{
        //Una vez establecida el valor del contador, cuando se pulse cualquiera de los botones se empezara a contar
        tmp_start();
    }

}


int main(void)
{
    irclk_ini(); // Enable internal ref clk to use by LCD

    led_green_ini();
    led_red_ini();
    sws_ini();
    lcd_ini();
    tpm_ini();
    //inicializamos el marcador
    min=0;
    seg=0;
    done=false; //sera true cuando se establezca el tiempo
    counted=false; //sera true cuando se haya acabado de contar
    lcd_display_time(min, seg);



    //mientras no haya un tiempo establecido
    while(!done){
        //cuando se haya establecido un tiempo done valdra true
        lcd_display_time(min, seg);
        delay();
        delay();
        lcd_ini();//clear
        delay();
    }
    lcd_display_time(min, seg);

    while(!counted){
        //cuando se haya  llegado al min=0 y seg=0 counted valdra true
    }
    //paramos el timer tmp
    TPM0->SC = 0;


    return 0;
}