#include "MKL46Z4.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "lcd.h"
#include "semphr.h"

#define P_PRODUCTOR 1 //Prioridad del productor
#define P_CONSUMIDOR 1 //Prioridad del consumidor

QueueHandle_t queueHandle;
int counter;
int numero;
TaskHandle_t tLCD = NULL;
TaskHandle_t tProd = NULL;
TaskHandle_t tCons = NULL;
SemaphoreHandle_t countMutex;

void irclk_ini()
{
    MCG->C1 = MCG_C1_IRCLKEN(1) | MCG_C1_IREFSTEN(1);
    MCG->C2 = MCG_C2_IRCS(0); //0 32KHZ internal reference clock; 1= 4MHz irc
}

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

void taskConsumidor(void *pvParameters)
{
    for (;;) {

        xSemaphoreTake(countMutex, 0);
        led_red_toggle();
        xQueueReceive(queueHandle, &numero, 0);
        counter--;
        xSemaphoreGive(countMutex);

        vTaskDelay(150/portTICK_RATE_MS);


    }
}

void taskProductor(void *pvParameters)
{
    int i=0;
    for (;;) {
        i++;

        xSemaphoreTake(countMutex, 0);
        led_green_toggle();
        xQueueSend( queueHandle,  &i, 0);
        counter++;
        xSemaphoreGive(countMutex);

        vTaskDelay(100/portTICK_RATE_MS);

    }
}



void taskLCD(void *pvParameters)
{
    for (;;) {

        /*
        UBaseType_t i=uxQueueSpacesAvailable( queueHandle );
        lcd_display_dec(9999-(int)i);
        lcd_display_dec(counter);
        vTaskDelay(10/portTICK_RATE_MS);
         */



        xSemaphoreTake(countMutex, 0);
        lcd_display_dec(counter);
        /*
        if(counter<0){
            lcd_display_dec(0);
        }else if(counter>9999){
            lcd_display_dec(9999);
        }else{
            lcd_display_dec(counter);
        }
         */

        xSemaphoreGive(countMutex);

        vTaskDelay(10/portTICK_RATE_MS);

    }
}



int main(void)
{
    irclk_ini();
    lcd_ini();
    led_green_init();
    led_red_init();
    lcd_display_dec(1234);
    counter=0;

    queueHandle = xQueueCreate(9999, 0);
    countMutex = xSemaphoreCreateMutex();
    if(countMutex==NULL){
        countMutex = xSemaphoreCreateMutex();
    }

    //Productor
    xTaskCreate(taskProductor, "TaskProductor",
                configMINIMAL_STACK_SIZE, (void *)NULL, 1, &tProd);


    //Consumidor
    xTaskCreate(taskConsumidor, "TaskConsumidor",
                configMINIMAL_STACK_SIZE, (void *)NULL, 1, tCons);


    //muestra el valor por el lcd
    xTaskCreate(taskLCD, "TaskLCD",
                configMINIMAL_STACK_SIZE, (void *)NULL, 1, &tLCD);

    /* start the scheduler */
    vTaskStartScheduler();

    /* should never reach here! */
    for (;;){}



    return 0;
}