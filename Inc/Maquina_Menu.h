#ifndef MAQUINA_MENU_H
#define MAQUINA_MENU_H
#include "Definiciones.h"
#include "ESTILO_PANTALLA.h"
#include "Logica_Juego.h"
#include "main.h"
class Maquina_Menu {
private:
   Estilo_Pantalla* _pantalla;
   Logica_Juego* _juego;
   EstadoMenu _estadoActual;
   OpcionJugadores _selJugadores;
   OpcionModo      _selModo;
   OpcionDificultad _selDificultad;
   uint32_t _tickParpadeo;
   bool _textoVisible;
   bool _inicioDibujado;
   uint32_t _tiempoBloqueo; 
   void dibujarPantallaInicio();
   void dibujarMenuJugadores();
   void dibujarMenuModo();
   void dibujarMenuDificultad();
   void dibujarCarga();
public:
   Maquina_Menu(Estilo_Pantalla* pantalla, Logica_Juego* juego);
   void actualizar(const Entradas& inputs);
   EstadoMenu getEstado() { return _estadoActual; }
};
#endif
