/*
* Maquina_estados.c
*
*  Created on: Dec 31, 2025
*      Author: LENOVO
*/
#include "Maquina_estados.h"
#include <stdlib.h>
#include<stdint.h>
#include "Calculo_puntos.h"
/* ====== Handles externos ====== */
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim4;
/* ====== Variables privadas ====== */
/* ====== Variables Globales (Visibles para otros bloques y Live Expressions) ====== */
// No llevan 'static' para que el Linker las encuentre desde los otros .c
EstadoJuego_t estado_juego;
ModoJuego_t modo;
uint8_t ganador;
uint8_t boton_pulsado;
uint32_t resultado_final;
// Usamos estos nombres para que coincidan con Calculo_puntos.c
uint32_t puntos_jugador_1;
uint32_t puntos_jugador_2;
uint8_t ronda_actual;
uint8_t dificultad;
uint8_t num_jugadores;
uint32_t tiempo_espera;
/* ====== Variables Privadas (Solo para este archivo) ====== */
static uint32_t tick_inicio;

//Variables de prueba para comprobar los tiempos de reacción
static uint32_t tiempo_ronda_1;
static uint32_t tiempo_ronda_2;
static uint32_t tiempo_ronda_3;
static uint32_t tiempo_de_reaccion;
static uint32_t tiempo_de_encendido;
extern volatile uint32_t tiempo_ms;
/* ====== Funciones privadas ====== */
/* ====== API ====== */
void Juego_Init(void)
{
   estado_juego = ST_JUEGO_INICIO;
   ronda_actual = 1;
   puntos_jugador_1 = puntos_jugador_2 = 0;
}
void Juego_Run(void)
{
   switch (estado_juego)
   {
       case ST_JUEGO_INICIO:
           ronda_actual = 1;
           puntos_jugador_1 = puntos_jugador_2 = 0;
           estado_juego = ST_JUEGO_TIEMPO_ESPERA;
           break;
       case ST_JUEGO_TIEMPO_ESPERA:
           // Solo calculamos el tiempo la primera vez que entramos al estado
           if (tiempo_espera == 0) {
               // Evitar división por cero si dificultad no está seteada
               uint8_t div_dif = (dificultad > 0) ? dificultad : 1;
               tiempo_espera = (rand() % (2000 / div_dif)) + 1000;
               tick_inicio = HAL_GetTick();
           }
           // Espera no bloqueante: Solo avanza si pasó el tiempo aleatorio
           if (HAL_GetTick() - tick_inicio >= tiempo_espera) {
               // Aquí encenderías el LED físicamente
        	   tiempo_de_encendido = tiempo_ms;
        	   if(ronda_actual <= 3){
        		   estado_juego = ST_JUEGO_RONDA;
        		   tiempo_espera = 0; // Reset para la próxima ronda
        		   HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
        	   }
           }
           if(ronda_actual >3){
                 estado_juego = ST_JUEGO_FIN;
                 tiempo_espera = 0; // Reset para la próxima ronda
           }

           break;
       case ST_JUEGO_RONDA:
           if (!boton_pulsado) return;
           // Llamada unificada a Calculo_Puntos con el ID del ganador
           // Usamos la nueva firma: (tiempo, modo, jugador)
           Calculo_Puntos(resultado_final, modo, ganador);
           boton_pulsado = 0;
           // Lógica de rondas (hasta 3 según tu diagrama actualizado)
           HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
        	   if (ronda_actual == 1) {
        		   tiempo_ronda_1 = resultado_final-tiempo_de_encendido;
        		   tiempo_de_reaccion=tiempo_ronda_1;
        	   }
        	   if (ronda_actual == 2) {
        	       tiempo_ronda_2 = resultado_final-tiempo_de_encendido;
        	       tiempo_de_reaccion=tiempo_ronda_2;
        	   }
        	   if (ronda_actual == 3) {
        	        tiempo_ronda_3 = resultado_final-tiempo_de_encendido;
        	        tiempo_de_reaccion=tiempo_ronda_3;
        	   }
        	   ronda_actual++;
               estado_juego = ST_JUEGO_TIEMPO_ESPERA;

           break;

       case ST_JUEGO_FIN:
    	   HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
           // Espera 3 segundos antes de volver al menú/inicio (No bloqueante)
           if (HAL_GetTick() - tick_inicio >= 3000) {
               estado_juego = ST_JUEGO_INICIO;
           }
           break;
   }
}
void Juego_BotonPulsado(uint8_t jugador, uint32_t tiempo)
{
   ganador = jugador;
   resultado_final = tiempo;
   boton_pulsado = 1;
}
void Juego_SetModo(ModoJuego_t m)        { modo = m; }
void Juego_SetJugadores(uint8_t n)       { num_jugadores = n; }
void Juego_SetDificultad(uint8_t d)      { dificultad = d; }
EstadoJuego_t Juego_GetEstado(void)      { return estado_juego; }
ModoJuego_t Juego_GetModo(void)      { return modo; }

uint32_t GetTiempoReaccion(void)		{return tiempo_de_reaccion;}





