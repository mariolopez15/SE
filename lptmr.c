#include "MKL46Z4.h"
#include "lcd.h"
#include <stdbool.h>
#include "drivers/functions.h"

// LED (RG)
// LED_GREEN = PTD5 (pin 98)
// LED_RED = PTE29 (pin 26)

// SWICHES
// RIGHT (SW1) = PTC3 (pin 73)
// LEFT (SW2) = PTC12 (pin 88)

// Enable IRCLK (Internal Reference Clock)
// see Chapter 24 in MCU doc

bool pushed_lptmr; //0-> sw1 1->sw2
bool done_lptmr; //ya se ha establecido
bool counted_lptmr;
uint8_t min_lptmr;
uint8_t seg_lptmr;

/*
void lptmr_ini(){

    //INICIALIZAR EL PUERTO DEL TIMER
    SIM->SCGC5 |=  SIM_SCGC5_LPTMR_MASK;//activamos el modo LPTMR
    MCG->C2|=MCG_C2_IRCS_MASK;
    LPTMR0->CSR = LPTMR_CSR_TEN(0); //Deshabilitamos el LPTMR para poder configurarlo
    // Configure LPTMR
    // select 1 kHz LPO clock with prescale factor 0, dividing clock by 2
    // resulting in 500 Hz clock
    LPTMR0->PSR = LPTMR_PSR_PCS(1); //se escoge el reloj de 1kHz
    LPTMR0->PSR = LPTMR_PSR_PRESCALE(0); //se precala con valor de 2, por lo tanto la frecuencia es de 500Hz
    LPTMR0->CSR |= LPTMR_CSR_TIE_MASK; //Se activan las interrupciones
    LPTMR0->CMR = LPTMR_CMR_COMPARE(10000); //cuenta a la que se producira la excepcion


    NVIC_SetPriority(28, 0); //TPM0_IRQn es el 17
    //NVIC_ClearPendingIRQ(17);
    NVIC_EnableIRQ(28);
}
 */

void lptmr_ini(){

    SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
    SIM->SCGC5|=SIM_SCGC5_LPTMR_MASK;//activamos el modo LPTMR
    LPTMR0->CSR=0; //Reiniciamos los valores de la configracion
    LPTMR0->CSR |= LPTMR_CSR_TIE_MASK;//Se activan las interrupciones
    LPTMR0->PSR = LPTMR_PSR_PCS(1)|LPTMR_PSR_PBYP_MASK;
    LPTMR0->CMR=LPTMR_CMR_COMPARE(1000);

    NVIC_SetPriority(28, 0); //TPM0_IRQn es el 17
    //NVIC_ClearPendingIRQ(17);
    NVIC_EnableIRQ(28);
}


void lptmr_start(){
    LPTMR0->CSR |= LPTMR_CSR_TEN_MASK; // se habilita la LPTMR
}


void minus_time_lptmr(){

    if(seg_lptmr==0){
        if(min_lptmr!=0){
            min_lptmr--;
            seg_lptmr=59;
        }else{
            min_lptmr=0;
            seg_lptmr=0;
            counted_lptmr=true;
        }
    }else{
        seg_lptmr--;
    }

}

void LPTMRIntHandler() {
    NVIC_ClearPendingIRQ(28);
    //PORTA->PCR[0] |=PORT_PCR_ISF(1);
    LPTMR0->CSR |= LPTMR_CSR_TCF_MASK;
    minus_time_lptmr();
    lcd_display_time(min_lptmr, seg_lptmr);

}


void PORTDIntHandler(void) {
    PORTC->PCR[3] |=PORT_PCR_ISF(1);
    PORTC->PCR[12] |=PORT_PCR_ISF(1);
    if(!done_lptmr){
        //Cuando el tiempo aun no este establecido, es decir cuando se pulsen los botones para establecer el valor del contador
        if(sw1_check() && sw2_check()){
            //Si estan pulsados los dos se establece el tiempo
            if(pushed_lptmr==0){
                /* Si el ultimo en ser pulsado fue el sw1(pushed_lptmr == 0) quiere decir que se
                 * pulso primero el sw2 y por lo tanto esa pulsacion, al no estar aun los
                 * dos pulsados a la vez, se contó. Por eso ahora se descuenta */

                if(seg_lptmr==0){
                    //el anterior a 0 es 59
                    seg_lptmr=59;
                }else{
                    seg_lptmr--;
                }

            }else{
                if(min_lptmr==0){
                    min_lptmr=59;
                }else{
                    min_lptmr--;
                }
            }

            done_lptmr=true;

        }else{
            //Cuando solo esta pulsado uno en un instante se actualiza el tiempo
            if(sw1_check()){
                //si esta pulsado se suma un seg_lptmrundo al tiempo
                pushed_lptmr=0;
                seg_lptmr++;
                if(seg_lptmr==60){
                    seg_lptmr=0;
                }
            }else if(sw2_check()){
                //Si esta pulsaodo se suma un min_lptmruto al tiempo
                pushed_lptmr=1;
                min_lptmr++;
                if(min_lptmr==60){
                    min_lptmr=0;
                }
            }
        }

    }else{
        //Una vez establecida el valor del contador, cuando se pulse cualquiera de los botones se empezara a contar
        lptmr_start();
    }

}


int main(void) {
    irclk_ini(); // Enable internal ref clk to use by LCD

    sws_ini();
    lcd_ini();
    lptmr_ini();
    //inicializamos el marcador
    min_lptmr = 0;
    seg_lptmr = 0;
    done_lptmr = false; //sera true cuando se establezca el tiempo
    counted_lptmr = false; //sera true cuando se haya acabado de contar
    lcd_display_time(min_lptmr, seg_lptmr);



    //mientras no haya un tiempo establecido
    while (!done_lptmr) {
        //cuando se haya establecido un tiempo done_lptmr
        //valdra true
        lcd_display_time(min_lptmr, seg_lptmr);
        delay();
        delay();
        lcd_ini();//clear
        delay();
    }
    lcd_display_time(min_lptmr, seg_lptmr);

    while (!counted_lptmr) {
        //cuando se haya  llegado al min_lptmr=0 y seg_lptmr=0 counted_lptmr valdra true
    }
    //paramos el timer tmp
    LPTMR0->CSR &= ~LPTMR_CSR_TEN_MASK;
    //LPTMR0->CSR = ~LPTMR_CSR_TEN(0);


    return 0;
}