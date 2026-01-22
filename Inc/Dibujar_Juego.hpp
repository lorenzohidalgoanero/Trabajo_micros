#ifndef DIBUJAR_JUEGO_HPP
#define DIBUJAR_JUEGO_HPP

#include "ESTILO_PANTALLA.h"
#include "Definiciones.h"
#include "GestorPuntuacion.hpp"

class DibujarJuego {
public:
    DibujarJuego(Estilo_Pantalla* pantalla);

    void dibujarEstado(EstadoJuego estado, const GestorPuntuacion& puntos, OpcionJugadores nJug);

private:
    Estilo_Pantalla* _pantalla;
    void dibujarFalso(uint16_t color);
    void dibujarMarcador(const GestorPuntuacion& puntos, OpcionJugadores nJug);
};

#endif
