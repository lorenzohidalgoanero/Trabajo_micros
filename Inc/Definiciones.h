#ifndef DEFINICIONES_H
#define DEFINICIONES_H

#include <stdint.h>
#include <stdbool.h>

// estados generales de la consola menu principal
enum class EstadoMenu {
    APAGADO= 0,
    SEL_JUGADORES,  // aqui eliges si juegan uno o dos
    SEL_MODO,       // eliges si es cuenta atras o mejor resultado
    SEL_DIFICULTAD, // facil medio o dificil
    PANTALLA_CARGA, // pantalla previa antes de empezar a jugar
    JUGANDO         // estado donde corre todo el juego
};

// opciones que se pueden seleccionar en los menus
enum class OpcionJugadores { UN_JUGADOR, DOS_JUGADORES };
enum class OpcionModo      { CUENTA_ATRAS, MEJOR_RESULTADO };
enum class OpcionDificultad{ FACIL, MEDIO, DIFICIL };

// estructura para pasar las entradas a la logica sin liar el codigo con hal gpio
struct Entradas {
    bool btn_PA0_Click;   // pulsacion corta confirmar o encender
    bool btn_PA0_Hold;    // pulsacion larga por si quieres usarla para encendido
    bool btn_Atras;       // volver al menu anterior
    bool btn_Salir;       // salida total vuelve al estado apagado
    bool btn_ReiniciarJuego; // reinicia la partida sin salir del menu

    // joystick por ahora simulado como banderas
    bool joy_Arriba;
    bool joy_Abajo;
};

// estados internos del juego una vez arrancado
enum class EstadoJuego {
    ST_JUEGO_APAGADO,            // el juego no esta activo
    ST_JUEGO_INICIO,             // inicio de partida
    ST_TIEMPO_PREPARACION,       // tiempo previo antes del disparo
    ST_TIEMPO_DISPARO,           // momento en el que se lanza el disparo
    ST_JUEGO_RONDA,              // ronda en curso
    ST_MOSTRAR_RESULTADO_RONDA,  // se muestran los resultados de la ronda
    ST_JUEGO_FIN                 // fin de la partida
};

#endif
