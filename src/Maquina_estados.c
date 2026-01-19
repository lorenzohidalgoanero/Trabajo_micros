/*
 * Maquina_estados.c
 *
 *  Created on: Dec 31, 2025
 *      Author: LENOVO
 */

#include "Maquina_estados.h"
#include <stdlib.h>
#include<stdint.h>


/* ====== Handles externos ====== */
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim4;

/* ====== Variables privadas ====== */
static EstadoJuego_t estado_juego;

static uint8_t ronda_actual;
static uint16_t puntos_j1, puntos_j2;
static uint8_t ganador;
static uint8_t boton_pulsado;
//static uint32_t tiempo_ms;
static uint32_t resultado_final;

static uint8_t dificultad;
static uint8_t num_jugadores;
static ModoJuego_t modo;

static uint32_t tick_inicio;
static uint32_t tiempo_espera;

/* ====== Funciones privadas ====== */
static uint16_t calcular_puntos(uint32_t tiempo)
{
    if (tiempo < 200) return 100;
    if (tiempo < 400) return 75;
    if (tiempo < 600) return 50;
    if (tiempo < 1000) return 25;
    return 10;
}

/* ====== API ====== */
void Juego_Init(void)
{
    estado_juego = ST_JUEGO_INICIO;
    ronda_actual = 1;
    puntos_j1 = puntos_j2 = 0;
}

void Juego_Run(void)
{
    switch (estado_juego)
    {
        case ST_JUEGO_INICIO:
            ronda_actual = 1;
            puntos_j1 = puntos_j2 = 0;
            estado_juego = ST_JUEGO_TIEMPO_ESPERA;
            break;

        case ST_JUEGO_TIEMPO_ESPERA:
            tiempo_espera = (rand() % (2000 / dificultad)) + 1000;
            tick_inicio = HAL_GetTick();
            estado_juego = ST_JUEGO_RONDA;
            break;

        case ST_JUEGO_RONDA:
            /* Espera no bloqueante */
            if (!boton_pulsado)
                return;

            switch (modo)
            {

                case MODO_3_DUO_PUNTOS:
                    if (ganador == 1) puntos_j1 += calcular_puntos(resultado_final);
                    else if (ganador == 2) puntos_j2 += calcular_puntos(resultado_final);
                    break;
                default:
                    break;
            }

            boton_pulsado = 0;

            if (ronda_actual++ < 3)
                estado_juego = ST_JUEGO_TIEMPO_ESPERA;
            else
                estado_juego = ST_JUEGO_FIN;
            break;

        case ST_JUEGO_FIN:
            HAL_Delay(3000);
            estado_juego = ST_JUEGO_INICIO;
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

