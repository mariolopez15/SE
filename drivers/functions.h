//
// Created by mario on 30/11/22.
//

#ifndef BARE_METAL_ARM_FUNCTIONS_H
#define BARE_METAL_ARM_FUNCTIONS_H

void irclk_ini();
void delay(void);
void sw1_ini();
void sw2_ini();
int sw1_check();
int sw2_check();
void sws_ini();
void led_green_ini();
void led_green_toggle();
void led_green_clear();
void led_green_set();
void led_red_ini();
void led_red_toggle(void);
void led_red_clear(void);
void led_red_set(void);
void leds_ini();

#endif //BARE_METAL_ARM_FUNCTIONS_H
