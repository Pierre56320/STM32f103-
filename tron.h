/*
 * snake.h
 *
 *  Created on: 12 janv. 2019
 *      Author: Pierre
 */

#ifndef TRON_H_
#define TRON_H_
#include "stm32f1xx_hal.h"
#include "config.h"
#include <stdbool.h>

void tron_start(bool multijoueur); //debute le jeu
void set_direction(uint8_t direction);

#endif /* TRON_H_ */
