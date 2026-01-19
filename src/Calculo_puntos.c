#include "stm32f4xx_hal.h"
#include "Calculo_puntos.h"
#include <stdint.h>

/* Usamos extern para que sean las mismas variables que en Maquina_estados.c */
extern uint32_t puntos_jugador_1;
extern uint32_t puntos_jugador_2;
extern uint8_t ganador; // Recibido desde Maquina_estados o Captura_boton

static uint16_t calcular_puntos_tiempo(uint32_t tiempo)
{
    if (tiempo < 200) return 100;
    if (tiempo < 400) return 75;
    if (tiempo < 600) return 50;
    if (tiempo < 1000) return 25;
    return 10;
}

/* Modificamos la función para que reciba el ID del ganador directamente */
/* En el archivo Calculo_puntos.h */
/* Elimina el punto y coma que estaba después de 'jugador_ganador)' */
void Calculo_Puntos(uint32_t tiempo_final, ModoJuego_t modo_actual, uint8_t jugador_ganador)
{
    switch (modo_actual)
    {
        case MODO_1_SOLO_TIEMPO:
            // En modo individual, siempre sumamos al jugador 1
            puntos_jugador_1 += calcular_puntos_tiempo(tiempo_final);
            break;

        case MODO_4_DUO_REFLEJOS:
            // El que pulsó primero gana 1 punto fijo
            if (jugador_ganador == 1) {
                puntos_jugador_1++;
            } else if (jugador_ganador == 2) {
                puntos_jugador_2++;
            }
            break;

        case MODO_3_DUO_PUNTOS:
            // Se suman puntos según la rapidez a quien haya ganado la pulsación
            if (jugador_ganador == 1) {
                puntos_jugador_1 += calcular_puntos_tiempo(tiempo_final);
            } else if (jugador_ganador == 2) {
                puntos_jugador_2 += calcular_puntos_tiempo(tiempo_final);
            }
            break;

        default:
            break;
    }
}
