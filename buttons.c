#include <tron.h>
#include "config.h"
#include "stm32f1_gpio.h"

void BTN_init(void){
	//Initialisation des boutons (carte Bluepill)
	BSP_GPIO_PinCfg(BUTTON_1,GPIO_MODE_IT_RISING_FALLING,GPIO_NOPULL,GPIO_SPEED_FREQ_HIGH);
	BSP_GPIO_PinCfg(BUTTON_2,GPIO_MODE_IT_RISING_FALLING,GPIO_NOPULL,GPIO_SPEED_FREQ_HIGH);
	BSP_GPIO_PinCfg(BUTTON_3,GPIO_MODE_IT_RISING_FALLING,GPIO_NOPULL,GPIO_SPEED_FREQ_HIGH);
	BSP_GPIO_PinCfg(BUTTON_4,GPIO_MODE_IT_RISING_FALLING,GPIO_NOPULL,GPIO_SPEED_FREQ_HIGH);

	HAL_NVIC_EnableIRQ((IRQn_Type)(EXTI15_10_IRQn));
}


/*
 * à chaque it, on met a jour la direction des joueurs
 */
void EXTI15_10_IRQHandler(void){
	if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_13) != RESET)
	{
		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_13);
		//On acquitte l’it
		//Front descendant - le bouton est appuyé – On lit 0 sur le port.
		if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13) == 0){
			set_direction(4);
		}
	}
	else if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_14) != RESET)
	{
		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_14);
		//On acquitte l’it
		//Front descendant - le bouton est appuyé – On lit 0 sur le port.
		if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14) == 0){
			set_direction(3);
		}
	}
	else if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_15) != RESET)
		{
			__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_15);
			//On acquitte l’it
			//Front descendant - le bouton est appuyé – On lit 0 sur le port.
			if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15) == 0){
				set_direction(2);
			}
		}
	else if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_12) != RESET)
		{
			__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_12);
			//On acquitte l’it
			//Front descendant - le bouton est appuyé – On lit 0 sur le port.
			if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12) == 0){
				set_direction(1);
			}
		}
}

