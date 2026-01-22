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
#include "Driver_Pantalla.h"
#include "ESTILO_PANTALLA.h"
#include "Logica_Juego.h"
#include "Maquina_Menu.h"
#include "Definiciones.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */
/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */
/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */
/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */
/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;
/* USER CODE BEGIN PV */
Driver_Pantalla* miPantalla;
Estilo_Pantalla* graficos;
Logica_Juego* juego;
Maquina_Menu* consola;
volatile bool irq_pa0_pressed = false;
/* USER CODE END PV */
/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */
/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
// 1. Interrupción de GPIO (Botones)
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if (GPIO_Pin == GPIO_PIN_0) {
	        irq_pa0_pressed = true;
	    }
}
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
 while (1)
 {
   /* USER CODE END WHILE */
  /* USER CODE BEGIN 3 */
	  // limpiamos inputs al inicio
		memset(&inputs, 0, sizeof(Entradas));
		// lógica de Botón para evitar problemas con el menu, tiene q ser robusto pq fastidia la logica
		if (irq_pa0_pressed) {
															// CAMBIAR ESTO POR UN HALGETTICK
			HAL_Delay(50);
			// comprobamos si sigue pulsado de verdad
			if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET) {
				uint32_t tiempo_inicio = HAL_GetTick();
				bool es_hold = false;
				// medimos el tiempo 								(Discriminador Click vs Hold)
				while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET) {
					// si pasa de 600ms, ya es HOLD. Salimos.
					if ((HAL_GetTick() - tiempo_inicio) > 600) {
						es_hold = true;
						inputs.btn_PA0_Hold = true;
						break;
					}
					HAL_Delay(10);
				}
				// si no fue Hold, fue Click
				if (!es_hold) {
					inputs.btn_PA0_Click = true;
				}
				// ESPERAR A SOLTAR (Wait for Release)
				// el código se congela aquí hasta que levantas el dedo
				while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET) {
					HAL_Delay(10);
				}
				HAL_Delay(100);
			}
			// LIMPIEZA FINAL
			// limpiamos la bandera de la interrupcion.
			__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_0);
			irq_pa0_pressed = false;
		}
		// IMPORTANTISIMA LINEA: lo que hacemos es pasar inputs al menú
		consola->actualizar(inputs);
		HAL_Delay(20);
 }
 /* USER CODE END 3 */
}
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
 RCC_OscInitStruct.PLL.PLLN = 50;
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
 if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
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
 /* 1. Activar los relojes de los puertos A y B */
 __HAL_RCC_GPIOA_CLK_ENABLE();
 __HAL_RCC_GPIOB_CLK_ENABLE();
 /* --- PANTALLA (PB0 y PB1) --- */
 /* PB0 = Reset Pantalla */
 /* PB1 = DC Pantalla */
 HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1, GPIO_PIN_RESET);
 GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
 GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; // salida Push-Pull
 GPIO_InitStruct.Pull = GPIO_NOPULL;
 GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
 HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
 /* el botón azul de la F411 Discovery es PA0 */
 GPIO_InitStruct.Pin = GPIO_PIN_0;
 GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING; // interrupción por flanco de subida
 GPIO_InitStruct.Pull = GPIO_PULLDOWN;       // resistencia interna a tierra
 HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
 /* --- ACTIVAR NVIC (Interrupciones) --- */
 /* la línea 0 atiende tanto a PA0 como a PB0, pero hemos configurado PA0 */
 HAL_NVIC_SetPriority(EXTI0_IRQn, 1, 0);
 HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}
/* USER CODE BEGIN 4 */
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

