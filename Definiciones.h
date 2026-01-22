#ifndef DEFINICIONES_H
#define DEFINICIONES_H

#include <stdint.h>
#include <stdbool.h>

// estados de la consola y del menu principal basicamente es donde esta el sistema en cada momento
enum class EstadoMenu {
   APAGADO = 0,        // consola apagada del todo no hace nada
   SEL_JUGADORES,     // aqui eliges si juega uno o dos jugadores
   SEL_MODO,          // eliges el modo de juego el que mas te mole
   SEL_DIFICULTAD,    // facil medio o dificil segun lo valiente que seas
   PANTALLA_CARGA,    // pantalla de empezar o cargando algo
   JUGANDO            // el juego ya esta en marcha y la logica corre
};

// opciones que se pueden seleccionar en los menus
enum class OpcionJugadores {
   UN_JUGADOR,    // juega una persona sola
   DOS_JUGADORES  // juegan dos personas
};

enum class OpcionModo {
   CUENTA_ATRAS,      // hay un tiempo limite y gana el mas rapido
   MEJOR_RESULTADO    // se guarda el mejor tiempo
};

enum class OpcionDificultad {
   FACIL,     // modo facil sin mucha complicacion
   MEDIO,     // modo normal de toda la vida
   DIFICIL    // modo dificil aqui ya hay que espabilar
};


struct Entradas {
   bool btn_PA0_Click;        // pulsacion normal confirmar encender etc
   bool btn_PA0_Hold;         // mantener pulsado durante un rato
   bool btn_Atras;            // volver atras en el menu
   bool btn_Salir;            // salir de todo y volver al apagado
   bool btn_ReiniciarJuego;   // reinicia solo la partida actual

   bool joy_Arriba;           // mover hacia arriba
   bool joy_Abajo;            // mover hacia abajo
};

// estados internos del juego no del menu
enum class EstadoJuego {
   INIT_LOCAL = 0,    // estado inicial esperando a empezar
   ESPERANDO_AZAR,    // espera un tiempo aleatorio antes del ya
   ACCION,            // momento ya aqui se mide el tiempo
   RESULTADO,         // se muestra el resultado o el ganador
   GAMEOVER           // fin de la partida se puede reiniciar
};

#endif
