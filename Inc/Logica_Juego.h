#ifndef LOGICA_JUEGO_H
#define LOGICA_JUEGO_H

#include "ESTILO_PANTALLA.h"
#include "Definiciones.h"
#include "GestorPuntuacion.hpp"
#include "Dibujar_Juego.hpp"
#include "main.h"

class Logica_Juego {
public:
    Logica_Juego(Estilo_Pantalla* pantalla);
    void configurar(OpcionJugadores jug, OpcionModo modo, OpcionDificultad dif);
    void resetearJuegoCompleto();
    void actualizar(const Entradas& inputs);

    EstadoJuego getEstadoActual() const { return _estadoActual; }

    bool getSolicitudSalidaMenu() const { return _solicitudSalidaMenu; }
    void setSolicitudSalidaMenu(bool valor) { _solicitudSalidaMenu = valor; }


private:
    Estilo_Pantalla* _pantalla;

    // Dibujante se encarga de pintar las escenas del juego (pantallas y marcador).
    DibujarJuego _dibujante;
    // Gestor de puntuación para llevar el registro de rondas y puntos de cada jugador.
    GestorPuntuacion _puntuacion;

    OpcionJugadores _confJugadores;
    OpcionModo _confModo;
    OpcionDificultad _confDificultad;

    bool _solicitudSalidaMenu;
    EstadoJuego _estadoActual;
    bool _redibujar;

    uint32_t _tickInicio;
    uint32_t _tickInicioDisparo;
    uint32_t _tiempoAleatorio;
    uint32_t _tiempoReaccion_1;
    uint32_t _tiempoReaccion_2;

    //flags para el modo 2 jugadores
    bool flag_jugador_1 = 0;
    bool flag_jugador_2 = 0;


    void Buzzer_Beep(uint16_t duracion);

    // Métodos auxiliares
    void visualizar();
    void iniciarRonda();
    uint32_t obtenerTiempoEspera();
};

#endif
