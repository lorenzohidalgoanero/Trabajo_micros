/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdint.h>

#include "main.h"
#include "DRIVER_PANTALLA.h"
#include "ESTILO_PANTALLA.h"
#include "Logica_Juego.h"
#include "Maquina_Menu.h"
#include "Definiciones.h"
#include "Captura_boton.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

// MIRAR EL MAIN.H PARA EL TEMA DE LOS BOTONES Y LEDS GPIO DEL JUEGO -> USER CODE BEGIN Private defines


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;
ADC_HandleTypeDef hadc1;
TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */
Driver_Pantalla* miPantalla;
Estilo_Pantalla* graficos;
Logica_Juego* juego;
Maquina_Menu* consola;

volatile bool irq_pa0_pressed = false;
volatile bool Presiona_Jugador_1 = false;
volatile bool Presiona_Jugador_2 = false;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM2_Init(void);


/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// 1. Interrupción de GPIO (Botones)
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if (GPIO_Pin == GPIO_PIN_0) {
	        irq_pa0_pressed = true;
	    }
	if (GPIO_Pin == GPIO_PIN_6) {            //J1
			Presiona_Jugador_1 = true;
			    }
	if (GPIO_Pin == GPIO_PIN_4) {            //J2
			Presiona_Jugador_2 = true;
			    }

	}

void Buzzer_Beep(uint16_t duracion_ms);

/* USER CODE END 0 */


/**
  * @brief  The application entry point.
  * @retval int
  */



int main(void)
{

  /* USER CODE BEGIN 1 */


  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_ADC1_Init();
  MX_TIM2_Init();





  /* USER CODE BEGIN 2 */

  	  	  	  	  	  	  	  	  	  //PARA LA PANTALLA
    // pasamos el SPI (&hspi1) y el tamaño (240x240) al driver y lo configuramos
    miPantalla = new Driver_Pantalla(&hspi1, 240, 240);
    miPantalla->inicializar();
    miPantalla->rellenarPantalla(0x0005);// si queremos 0x0000 negro puro.
    //Para el dibujo en si, no deberia llamarse ESTILO PANTALLA sino dibujante o algo asi
    graficos = new Estilo_Pantalla(miPantalla, 2); // Fuente tamaño medio
    juego = new Logica_Juego(graficos);
    consola = new Maquina_Menu(graficos, juego);

    Captura_Boton_Init();          // Configura TIM/ADC para la captura de tiempos
    juego->resetearJuegoCompleto(); // Deja el juego en estado inicial/coherente

      Entradas inputs = {0};


    /*
    // Colores para hacer la letra retro
    uint16_t COLOR_FRENTE = 0xFFE0; // Amarillo brillante
    uint16_t SOMBRA_DER   = 0xF800; // Rojo
    uint16_t SOMBRA_ABAJO = 0x001F; // Azul

    // Sintaxis: Escribir(x, y, "TEXTO", ColorFrente, ColorSombraDer, ColorSombraAbajo)
    textoGlitch->Escribir(50, 75, "GAME", COLOR_FRENTE, SOMBRA_DER, SOMBRA_ABAJO);
    textoGlitch->Escribir(50, 125, "OVER", COLOR_FRENTE, SOMBRA_DER, SOMBRA_ABAJO);
*/


    /* USER CODE END 2 */



      /* Infinite loop */
      /* USER CODE BEGIN WHILE */
      while (1) {
            memset(&inputs, 0, sizeof(Entradas));

            if (irq_pa0_pressed) {
                // Antirrebote inicial
                uint32_t Tiempo_While = HAL_GetTick();
                while (HAL_GetTick() - Tiempo_While < 40);

                if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET) {
                    uint32_t t_inicio = HAL_GetTick();

                    // ESPERA SILENCIOSA: No llamamos a actualizar aquí para no saltar estados
                    while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET) {
                        if ((HAL_GetTick() - t_inicio) > 600) {
                            inputs.btn_PA0_Hold = true;
                            break;
                        }
                    }

                    // Si soltó antes de 600ms, es un Click
                    if (!inputs.btn_PA0_Hold) {
                        inputs.btn_PA0_Click = true;
                    }

                    // ESPERA A SOLTAR DEFINITIVAMENTE: Sin llamar a actualizar
                    while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET);
                }

                irq_pa0_pressed = false;
                __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_0);

                // Solo ahora que hemos soltado y decidido qué era (Click o Hold),
                // procesamos la lógica UNA VEZ.
                consola->actualizar(inputs);

                // Delay de cortesía para que la pantalla respire tras el procesamiento
                HAL_Delay(150);
            }

            //BOTON JUGADOR 1

            if (Presiona_Jugador_1) {
                // Antirrebote inicial
                uint32_t Tiempo_While = HAL_GetTick();
                while (HAL_GetTick() - Tiempo_While < 40);

                if (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_6) == GPIO_PIN_SET) {
                    uint32_t t_inicio = HAL_GetTick();
                    inputs.Boton_1 = true;

                }

                Presiona_Jugador_1 = false;
                __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_6);

                // Solo ahora que hemos soltado y decidido qué era (Click o Hold),
                // procesamos la lógica UNA VEZ.
                consola->actualizar(inputs);

                // Delay de cortesía para que la pantalla respire tras el procesamiento
                HAL_Delay(150);
            }

            //BOTON JUGADOR 2

            if (Presiona_Jugador_2) {
                // Antirrebote inicial
                uint32_t Tiempo_While = HAL_GetTick();
                while (HAL_GetTick() - Tiempo_While < 40);

                if (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_4) == GPIO_PIN_SET) {
                    uint32_t t_inicio = HAL_GetTick();
                    inputs.Boton_2 = true;

                }

                Presiona_Jugador_2 = false;
                __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_4);

                // Solo ahora que hemos soltado y decidido qué era (Click o Hold),
                // procesamos la lógica UNA VEZ.
                consola->actualizar(inputs);

                // Delay de cortesía para que la pantalla respire tras el procesamiento
                HAL_Delay(150);
            }
            else {
                // Si no hay interrupción, el menú sigue su curso normal (animaciones, etc.)
                consola->actualizar(inputs);
            }

            // Estabilidad del sistema
            uint32_t Tiempo_While_Loop = HAL_GetTick();
            while (HAL_GetTick() - Tiempo_While_Loop < 10);
        }

}




  /* USER CODE END 3 */


/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 100;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 8;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH; //ANTES ESTABA LOW
  hspi1.Init.CLKPhase = SPI_PHASE_2EDGE; //ANTES ESTABA 1
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1|BUZZER_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, LED_ROJO_Pin|LED_AZUL_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : BOTON_AZUL_Pin */
  GPIO_InitStruct.Pin = BOTON_AZUL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(BOTON_AZUL_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB1 BUZZER_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|BUZZER_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : LED_ROJO_Pin LED_AZUL_Pin */
  GPIO_InitStruct.Pin = LED_ROJO_Pin|LED_AZUL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : PA10 PA11 */
  GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : BTN_J2_Pin BTON_J1_Pin */
  GPIO_InitStruct.Pin = BTN_J2_Pin|BTON_J1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_10;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 83;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 999;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}



/* USER CODE BEGIN 4 */
void Buzzer_Beep(uint16_t duracion_ms)
{
    HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET);
    HAL_Delay(duracion_ms);
    HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
