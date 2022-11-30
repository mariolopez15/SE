#include "MKL46Z4.h"
#include "lcd.h"
#include <stdbool.h>
#include "functions.h"

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



void tpm_ini(){

    SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK; //iniacilizamos el puerto A que es donde esta el TPM0
    SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK; //activamos el reloj para el TPM
    SIM->SOPT2 |= SIM_SOPT2_TPMSRC(3); //trabajara con el MCGIRCLK que trabaja a 32 kHz
    TPM0->SC = 0; //deshabilitamos el tpm para modificar el PS
    TPM0->SC = (TPM_SC_PS(4)); //dividimos por 32
    //Como trabaja a 32Khz pero lo dividimos entre 32 pasara 1s al completar 1000 ciclos
    TPM0->MOD = TPM_MOD_MOD(2000); //El objetivo son 2000
    TPM0->SC |= TPM_SC_TOIE_MASK; //habilitamos que se produzca la interrupcion
    NVIC_SetPriority(17, 0); //TPM0_IRQn es el 17
    NVIC_EnableIRQ(17);
}

void tmp_start(){
    TPM0->SC |= TPM_SC_CMOD(1); //se establece el modo de conteo
}


void minus_time_tpm(){

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
    NVIC_ClearPendingIRQ(17);
    TPM0->SC |= TPM_SC_TOF_MASK; //al ser llamado la interrupcion pone el bit del overflow a 1 ya que el valor d ela cuenta alcanzo el objetivo
    minus_time_tpm();
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
