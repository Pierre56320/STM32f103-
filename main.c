/**
  ******************************************************************************
  * @file    main.c
  * @author  Samuel Poiraud
  * @version V2.0
  * @date    25/10/2017
  * @brief   Fonction main
  ******************************************************************************
*/
#include <tron.h>
#include "stm32f1xx_hal.h"
#include "stm32f1_uart.h"
#include "stm32f1_sys.h"
#include "macro_types.h"
#include "stm32f1_gpio.h"
#include "stm32f1_timer.h"
#include "stm32f1_ili9341.h"
#include "buttons.h"
#include "menu.h"



//initialise le programe et ouvre le menu
int main(void)
{
	HAL_Init();			//Initialisation de la couche logicielle HAL (Hardware Abstraction Layer)
	BSP_GPIO_Enable();	//Activation des périphériques GPIO
	SYS_ClockConfig();		//Configuration des horloges.
	BTN_init(); //Initialisation des boutons

	//Initialisation du port du Buzzer(carte Bluepill)
	//BSP_GPIO_PinCfg(BUZZER,GPIO_MODE_OUTPUT_PP,GPIO_PULLUP,GPIO_SPEED_FREQ_HIGH);


	XPT2046_init();
	ILI9341_Init();
	ILI9341_Rotate(ILI9341_Orientation_Landscape_1);
	menu_start(true);
}
