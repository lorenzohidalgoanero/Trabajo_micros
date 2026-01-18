/*
 * Cálculo_puntos.h
 *
 *  Created on: Dec 31, 2025
 *      Author: LENOVO
 */

#ifndef INC_CALCULO_PUNTOS_H_
#define INC_CALCULO_PUNTOS_H_

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include "Maquina_estados.h"

/* Variables públicas donde se guardan los tiempos */
extern uint32_t tiempo_jugador_1;
extern uint32_t tiempo_jugador_2;

/* Función principal del bloque */
void Calculo_Puntos(uint32_t tiempo_final);

#endif /* INC_CALCULO_PUNTOS_H_ */
