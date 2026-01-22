#ifndef LOGICA_JUEGO_H
#define LOGICA_JUEGO_H
#include "ESTILO_PANTALLA.h"
#include "Definiciones.h"
#include "main.h" // para HAL_GetTick
class Logica_Juego {
public:
   Logica_Juego(Estilo_Pantalla* pantalla);
   void configurar(OpcionJugadores jug, OpcionModo modo, OpcionDificultad dif);
   void resetearJuegoCompleto();
   void actualizar(const Entradas& inputs);
private:
   Estilo_Pantalla* _pantalla;
   OpcionJugadores _confJugadores;
   OpcionModo _confModo;
   OpcionDificultad _confDificultad;
   // Estado interno
   EstadoJuego _estadoActual;
   bool _redibujar;
   uint32_t _tickInicio;      
   uint32_t _tiempoAleatorio; 
   uint32_t _tiempoReaccion;     


   void visualizar();
   void iniciarRonda();
   uint32_t obtenerTiempoEspera();
};
#endif
