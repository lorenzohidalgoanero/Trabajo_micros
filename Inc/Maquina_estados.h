/*
 * Maquina_estados.h
 *
 *  Created on: Dec 31, 2025
 *      Author: LENOVO
 */

#ifndef MAQUINA_ESTADOS_H
#define MAQUINA_ESTADOS_H

#include "stm32f4xx_hal.h"
#include <stdint.h>

/* ====== Tipos ====== */
typedef enum {
    ST_JUEGO_INICIO,
    ST_JUEGO_TIEMPO_ESPERA,
    ST_JUEGO_RONDA,
    ST_JUEGO_FIN
} EstadoJuego_t;

typedef enum {
    MODO_1_SOLO_TIEMPO,
    //MODO_2_SOLO_PUNTOS,
    MODO_3_DUO_PUNTOS,
    MODO_4_DUO_REFLEJOS
} ModoJuego_t;

/* ====== Interfaz pública ====== */
void Juego_Init(void);
void Juego_Run(void);

/* Comunicación con main / FSM superior */
void Juego_SetModo(ModoJuego_t modo);
void Juego_SetJugadores(uint8_t num);
void Juego_SetDificultad(uint8_t dif);

/* Usado por interrupciones */
void Juego_BotonPulsado(uint8_t jugador, uint32_t tiempo);

/* Estado */
EstadoJuego_t Juego_GetEstado(void);
uint32_t GetTiempoReaccion(void);


#endif

