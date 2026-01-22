
#include "Captura_boton.h"
#include "main.h"
#include <stdlib.h>
#include <stdint.h>
#include "stm32f4xx_hal.h"

// ESTO HAY QUE METERLO Y NOSE MUY BIEN PORQUE
extern "C" {
    extern TIM_HandleTypeDef htim2;
    extern ADC_HandleTypeDef hadc1;
}

// esto se actualiza con los callbackks
volatile uint32_t tiempo_ms = 0;
volatile uint16_t adc_ruido = 0;

static volatile uint8_t boton_pulsado = 0;
static volatile uint32_t resultado_final = 0;
static uint8_t ganador_local = 0;


extern volatile bool irq_pa0_pressed; 
// externs para traerlo del main
static bool temporizador_activo = false;

extern "C" void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM2) {
        tiempo_ms++;
    }
}


extern "C" void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
    if (hadc->Instance == ADC1) {
        adc_ruido = static_cast<uint16_t>(HAL_ADC_GetValue(hadc));
    }
}

// inicializa la captura del botón, arranca el ADC en modo interrupción y
// configura la semilla del generador aleatorio con el ruido inicial
void Captura_Boton_Init(void) {
    HAL_ADC_Start_IT(&hadc1);
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
        // solo arranca si el boton está SUELTO para evitar trampas
        if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET) return;

        tiempo_ms = 0;
        __HAL_TIM_SET_COUNTER(&htim2, 0);
        HAL_TIM_Base_Start_IT(&htim2);
        temporizador_activo = true;
        boton_pulsado = 0;
        resultado_final = 0;
        irq_pa0_pressed = false; // limpiamos posibles señales residuales del boton
    }

    if (boton_pulsado == 0) {
        // judador1 prioritario, detectamos si ocurrio en la ronda
        if (irq_pa0_pressed) {
            resultado_final = tiempo_ms;
            boton_pulsado = 1;
            ganador_local = 1;
            HAL_TIM_Base_Stop_IT(&htim2);
            temporizador_activo = false;
            irq_pa0_pressed = false;
            HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
        }
        // jugador2, polling  en PA10
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


// funciones auxiliares para consultar el resultado de la captura
// devuelven el tiempo registrado y el jugador que gano
uint32_t Captura_GetTiempoFinal() {
    return resultado_final;
}

uint8_t Captura_GetGanador() {
    return ganador_local;
}

