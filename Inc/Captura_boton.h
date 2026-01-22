
#ifndef INC_CAPTURA_BOTON_H_
#define INC_CAPTURA_BOTON_H_

#include <stdint.h>

void Captura_Boton_Init(void);
void Captura_Boton_Run(void);

// Devuelve el tiempo de reacción medido en milisegundos desde el inicio de la ronda.
uint32_t Captura_GetTiempoFinal();

// devuelve el jugador ganador, si no se ha registrado ninguna pulsación aun devuelve 0
uint8_t Captura_GetGanador();


#endif /* INC_CAPTURA_BOTON_H_ */
