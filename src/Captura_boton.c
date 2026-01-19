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

/* Handles externos */
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim4;
extern ADC_HandleTypeDef hadc1;

/* Variables externas (tal como las usabas) */
extern volatile uint32_t tiempo_ms;
 volatile uint8_t boton_pulsado=0;
 volatile uint32_t resultado_final=0;
extern volatile uint16_t adc_ruido;
 uint8_t modo_actual;

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
    /* 1. Limpieza inicial */
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1 | GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);

    boton_pulsado = 0;
    resultado_final = 0;

    /* 2. Obtener Azar */
    uint32_t tiempo_espera_azar = (rand() % 2001) + 2000;

    /* 3. Espera */
    uint32_t tiempo_inicio_espera = HAL_GetTick();

    /* 4. Inicio ronda */
    tiempo_ms = 0;


    __HAL_TIM_SET_COUNTER(&htim2, 0);
    HAL_TIM_Base_Start_IT(&htim2);

    /* 5. Espera pulsación */
    if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET)
    {
        while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET);
        HAL_Delay(100);

        while ((HAL_GetTick() - tiempo_inicio_espera) < tiempo_espera_azar)
        {
            HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);

            if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET)
            {
                boton_pulsado = 1;
                resultado_final = tiempo_ms;
                HAL_TIM_Base_Stop_IT(&htim2);

                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1 | GPIO_PIN_2, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);

                __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_3, 32767);
                HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);
            }
        }
    }

    resultado_final = tiempo_ms;
    HAL_TIM_Base_Stop_IT(&htim2);

    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1 | GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);

    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_3, 32767);
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);

    if (resultado_final == 0)
        resultado_final = 11111;
}

