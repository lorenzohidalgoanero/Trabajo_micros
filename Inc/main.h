/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <stdlib.h>
extern volatile uint32_t tiempo_ms;


/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define BOTON_AZUL_Pin GPIO_PIN_0
#define BOTON_AZUL_GPIO_Port GPIOA
#define BOTON_AZUL_EXTI_IRQn EXTI0_IRQn
#define BUZZER_Pin GPIO_PIN_14
#define BUZZER_GPIO_Port GPIOB
#define LED_ROJO_Pin GPIO_PIN_14
#define LED_ROJO_GPIO_Port GPIOD
#define LED_AZUL_Pin GPIO_PIN_15
#define LED_AZUL_GPIO_Port GPIOD
#define BTN_J2_Pin GPIO_PIN_4
#define BTN_J2_GPIO_Port GPIOD
#define BTN_J2_EXTI_IRQn EXTI4_IRQn
#define BTON_J1_Pin GPIO_PIN_6
#define BTON_J1_GPIO_Port GPIOD
#define BTON_J1_EXTI_IRQn EXTI9_5_IRQn

/* USER CODE BEGIN Private defines */


/*#define LED_AZUL_GPIO_Port  GPIOD					//para la logica del juego
#define LED_AZUL_Pin        GPIO_PIN_15
#define LED_ROJO_GPIO_Port  GPIOD
#define LED_ROJO_Pin        GPIO_PIN_14
#define BOTON_JUGADOR_1_GPIO_Port  GPIOD
#define BOTON_JUGADOR_1_Pin        GPIO_PIN_6
#define BOTON_JUGADOR_2_GPIO_Port  GPIOD
#define BOTON_JUGADOR_2_Pin        GPIO_PIN_4*/
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
