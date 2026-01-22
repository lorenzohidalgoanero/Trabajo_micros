

#ifndef INC_CAPTURA_BOTON_H_
#define INC_CAPTURA_BOTON_H_

#include <stdint.h>

void Captura_Boton_Init(void);
void Captura_Boton_Run(void);

// Funciones auxiliares para consultar el resultado de la última captura
// Devuelve el tiempo de reacción medido en milisegundos desde el inicio de la ronda.
uint32_t Captura_GetTiempoFinal();

// Devuelve el identificador del jugador ganador (1 o 2). Si no se ha registrado
// ninguna pulsación aún, devuelve 0.
uint8_t Captura_GetGanador();


#endif /* INC_CAPTURA_BOTON_H_ */

