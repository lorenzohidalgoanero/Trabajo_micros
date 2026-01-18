#include "Calculo_puntos.h"

/* Variables */
uint32_t tiempo_jugador_1 = 0;
uint32_t tiempo_jugador_2 = 0;
uint32_t puntos_jugador_1 = 0;
uint32_t puntos_jugador_2 = 0;
static uint8_t ganador;
volatile uint32_t tiempo_1 = 0;
volatile uint32_t tiempo_2 = 0;

void Calculo_Puntos(uint32_t tiempo_final, EstadoJuego_t estado_actual)
{
	EstadoJuego_t estado = Juego_GetEstado();
	ModoJuego_t modo = Juego_GetModo();

	switch (modo)
	{
	case MODO_1_SOLO_TIEMPO:
	            /* En ronda normal, guardamos el tiempo del jugador activo */
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
	        	            /* En ronda normal, guardamos el tiempo del jugador activo */
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

	        	            else						 //Si es más rápido el jugador 2 que el 1
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
	        		        /* En ronda normal, guardamos el tiempo del jugador activo */
	        		        if (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_14) == GPIO_PIN_SET)
	        		        {
	        		        	puntos_jugador_1 = puntos_jugador_1 + 1;
	        		        	break;
	        		        }
	        		        else if (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_15) == GPIO_PIN_SET)
	        		        {
	        		        	puntos_jugador_1 = puntos_jugador_1 + 1;
	        		        	break;
	        		        }
	        	}
	        default:
	            break;

	}

}
