
#include "Captura_boton.h"
#include "main.h"
#include <stdlib.h>
#include <stdint.h>
#include "stm32f4xx_hal.h"

// Referencias a handles de hardware proporcionados por STM32Cube.
// Por esto:
extern "C" {
    extern TIM_HandleTypeDef htim2;
    extern ADC_HandleTypeDef hadc1;
}

// Variables globales para almacenar el tiempo en milisegundos y
// el valor de ruido de ADC usados para la generación de números aleatorios.
// Estas variables se actualizan en las callbacks de HAL.
volatile uint32_t tiempo_ms = 0;
volatile uint16_t adc_ruido = 0;

// Variables internas para registrar el momento de la pulsación y el jugador ganador.
static volatile uint8_t boton_pulsado = 0;
static volatile uint32_t resultado_final = 0;
static volatile uint32_t resultado_final_1 = 0;
static volatile uint32_t resultado_final_2 = 0;
static uint8_t ganador_local = 0;


extern volatile bool irq_pa0_pressed; // Traemos el flag del main
extern volatile bool Presiona_Jugador_1;
extern volatile bool Presiona_Jugador_2;

// Indicador de si el temporizador está en marcha. Esto evita reiniciar
// accidentalmente el conteo en cada llamada a Run.
static bool temporizador_activo = false;

// Callback de HAL para el temporizador. Se llama cada vez que el temporizador
// genera una interrupción (configurado a 1 kHz para incrementar 1 ms).
extern "C" void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM2) {
        tiempo_ms++;
    }
}

// Callback de HAL para la conversión del ADC. Captura ruido para usarlo
// como semilla de números aleatorios.
extern "C" void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
    if (hadc->Instance == ADC1) {
        adc_ruido = static_cast<uint16_t>(HAL_ADC_GetValue(hadc));
    }
}

// Inicializa la captura del botón: arranca el ADC en modo interrupción y
// configura la semilla del generador aleatorio con el ruido inicial.
void Captura_Boton_Init(void) {
    HAL_ADC_Start_IT(&hadc1);
    // Permite que el ADC realice al menos una conversión antes de usar su ruido.
    HAL_Delay(10);
    srand(adc_ruido);
    boton_pulsado = 0;
    resultado_final = 0;
    ganador_local = 0;
    temporizador_activo = false;
    tiempo_ms = 0;
}




void Captura_Boton_Run(void) {
    if (!temporizador_activo) {
        // SEGURIDAD: Solo arranca si el botón está SUELTO para evitar trampas
        if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET) return;

        tiempo_ms = 0;
        __HAL_TIM_SET_COUNTER(&htim2, 0);
        HAL_TIM_Base_Start_IT(&htim2);
        temporizador_activo = true;
        boton_pulsado = 0;
        resultado_final = 0;
        irq_pa0_pressed = false; // Limpiamos basuras previas
    }

    if (boton_pulsado == 0) {
        // Prioridad J1: Detectamos si la interrupción ocurrió durante la ronda
        if (irq_pa0_pressed) {
            resultado_final = tiempo_ms;
            boton_pulsado = 1;
            ganador_local = 1;
            HAL_TIM_Base_Stop_IT(&htim2);
            temporizador_activo = false;
            irq_pa0_pressed = false;
            HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
        }
        // J2: Polling normal en PA10
        else if (Presiona_Jugador_1) {
        	resultado_final_1 = tiempo_ms;
        	boton_pulsado = 1;
        	ganador_local = 1;
        	HAL_TIM_Base_Stop_IT(&htim2);
        	temporizador_activo = false;
        	Presiona_Jugador_1 = false;
        	HAL_GPIO_WritePin(LED_ROJO_GPIO_Port, LED_ROJO_Pin, GPIO_PIN_SET);
               }
              // J2: Polling normal en PA10
         else if (Presiona_Jugador_2) {
        	resultado_final_2 = tiempo_ms;
        	boton_pulsado = 1;
        	ganador_local = 2;
        	HAL_TIM_Base_Stop_IT(&htim2);
        	temporizador_activo = false;
        	Presiona_Jugador_2 = false;
        	HAL_GPIO_WritePin(LED_ROJO_GPIO_Port, LED_ROJO_Pin, GPIO_PIN_SET);
         }
      }
   }



// Ejecuta la lógica de captura. Debe llamarse de manera periódica mientras
// dure la ronda. Cuando detecta una pulsación, detiene el temporizador
// e informa del resultado.
/*void Captura_Boton_Run(void) {
 *
 * FUNCIONA PERO PUEDO MEJORARLO
 *
    if (!temporizador_activo) {
        // SEGURIDAD: Si al empezar la ronda el botón ya está pulsado,
        // ignoramos esa pulsación (el usuario está haciendo trampa o no ha soltado).
        if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET) {
            return;
        }

        tiempo_ms = 0;
        __HAL_TIM_SET_COUNTER(&htim2, 0);
        HAL_TIM_Base_Start_IT(&htim2);
        temporizador_activo = true;
        boton_pulsado = 0;
        resultado_final = 0;
    }

    if (boton_pulsado == 0) {
        // J1
        if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET) {
            resultado_final = tiempo_ms;
            boton_pulsado = 1;
            ganador_local = 1;
            HAL_TIM_Base_Stop_IT(&htim2);
            temporizador_activo = false;
            HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
        }
        // J2
        else if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_10) == GPIO_PIN_SET) {
            resultado_final = tiempo_ms;
            boton_pulsado = 1;
            ganador_local = 2;
            HAL_TIM_Base_Stop_IT(&htim2);
            temporizador_activo = false;
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
        }
    }
}
*/


// Funciones auxiliares para consultar el resultado de la captura.
// Devuelven el tiempo registrado y el jugador que ganó.
uint32_t Captura_GetTiempoFinal() {
    return resultado_final;
}

uint8_t Captura_GetGanador() {
    return ganador_local;
}
