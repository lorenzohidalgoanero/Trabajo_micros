/*
* Captura_boton.c
*
*  Created on: Dec 31, 2025
*      Author: LENOVO
*/
#include "Captura_Boton.h"
#include <stdlib.h>
#include<stdint.h>
#include "stm32f4xx_hal.h"
#include "Maquina_estados.h"
/* Handles externos */
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim4;
extern ADC_HandleTypeDef hadc1;
/* Variables externas (tal como las usabas) */
extern volatile uint32_t tiempo_ms;
/*volatile*/ extern uint8_t boton_pulsado/*=0*/;
/*volatile*/ extern uint32_t resultado_final/*=0*/;
extern volatile uint16_t adc_ruido;
extern uint8_t ganador;
extern ModoJuego_t modo;
/* ===== INIT ===== */
void Captura_Boton_Init(void)
{
   HAL_ADC_Start_IT(&hadc1);
   HAL_Delay(100);
   srand(adc_ruido);
}
/* ===== RUN ===== */
void Captura_Boton_Run(void)
{
   extern EstadoJuego_t estado_juego;
   // Solo permitimos capturar si estamos en RONDA y el botón no ha sido procesado aún
   if (estado_juego == ST_JUEGO_RONDA && boton_pulsado == 0)
   {
	   HAL_TIM_Base_Start_IT(&htim2);
       // Si se detecta pulsación en PA0
       if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET)  //BOTÓN JUGADOR 1
       {
           // Captura inmediata para máxima precisión
           resultado_final = tiempo_ms;
           boton_pulsado = 1;
           ganador = 1;
           // Detenemos el hardware de conteo
           HAL_TIM_Base_Stop_IT(&htim2);
           // Debouncing mínimo (para la realidad física del botón)
           HAL_Delay(50);
           // Feedback: Encendemos un LED que indique "pulsación detectada"
           HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
       }
       // --- COMPROBACIÓN JUGADOR 2 (PA10) ---
          else if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_10) == GPIO_PIN_SET)   //BOTÓN JUGADOR 2
          {
                   resultado_final = tiempo_ms;
                   boton_pulsado = 1;
                   ganador = 2; // Registra que ganó el J2
                   // Detenemos el hardware de conteo
                   HAL_TIM_Base_Stop_IT(&htim2);
                   // Feedback visual: puedes encender otro LED (ej. PA1 o PA2)
                   // para saber que el J2 fue el más rápido
                   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
                   HAL_Delay(50); // Debouncing
             }
   }
}
