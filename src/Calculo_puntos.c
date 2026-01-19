/*
 * Cálculo_puntos.c
 *
 *  Created on: Dec 31, 2025
 *      Author: LENOVO
 */


#include "stm32f4xx_hal.h"
#include "Calculo_puntos.h"
#include <stdint.h>


/* Variables */
uint32_t tiempo_jugador_1;
uint32_t tiempo_jugador_2;
uint32_t puntos_jugador_1;
uint32_t puntos_jugador_2;
uint8_t ganador;
uint8_t punto_ganado;
volatile uint32_t tiempo_1;
volatile uint32_t tiempo_2;




void Calculo_Puntos(uint32_t tiempo_final, EstadoJuego_t estado_actual, ModoJuego_t modo_actual)
{


	switch (modo_actual)
	{
			case MODO_1_SOLO_TIEMPO:
	            	/* en ronda normal, guardamos el tiempo del jugador activo */
	            	if (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_14) == GPIO_PIN_SET)
	            	{
	            		tiempo_jugador_1 = tiempo_final;
	            	}
	            	else
	            	{
	            		tiempo_jugador_2 = tiempo_final;
	            	}
	            	break;


	        case MODO_4_DUO_REFLEJOS:
	        	switch (estado_actual)
	        	    {
	        	        case ST_JUEGO_RONDA:
	        	            /* en ronda normal, guardamos el tiempo del jugador activo */
	        	            if (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_14) == GPIO_PIN_SET)
	        	            {
	        	                tiempo_jugador_1 = tiempo_1;
	        	                break;
	        	            }
	        	            if (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_15) == GPIO_PIN_SET)
	        	            {
	        	                tiempo_jugador_2 = tiempo_2;
	        	            	break;
	        	            }

	        	            if (tiempo_1 <= tiempo_2 ) //Si es más rápido el jugador 1 que el 2
	        	            {
	        	                tiempo_jugador_1 = tiempo_final;
	        	                puntos_jugador_1 = puntos_jugador_1 +1;
	        	            }

	        	            else						 //si es más rápido el jugador 2 que el 1
	        	            {
	        	            	tiempo_jugador_2 = tiempo_final;
	        	            	puntos_jugador_2 = puntos_jugador_2 + 1;
	        	            }
	        	            break;

	        	    }
	            break;

	        case MODO_3_DUO_PUNTOS:
	        	switch (estado_actual)
	        	{
	        	case ST_JUEGO_RONDA:
	        		punto_ganado=0;
	        		ganador=0;
	        		        /* en ronda normal, guardamos el tiempo del jugador activo */
	        		        if (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_14) == GPIO_PIN_SET && punto_ganado==0)
	        		        {
	        		        	puntos_jugador_1 = puntos_jugador_1 + 1;
	        		        	punto_ganado=1;
	        		        	break;
	        		        }
	        		        if (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_15) == GPIO_PIN_SET && punto_ganado==0)
	        		        {
	        		        	puntos_jugador_2 = puntos_jugador_2 + 1;
	        		        	punto_ganado=1;
	        		        	break;
	        		        }
	        	}
	        default:
	            break;
	}

}
