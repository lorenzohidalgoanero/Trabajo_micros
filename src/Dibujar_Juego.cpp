#include "Dibujar_Juego.hpp"
#include <stdio.h>

DibujarJuego::DibujarJuego(Estilo_Pantalla* pantalla) : _pantalla(pantalla) {}

void DibujarJuego::dibujarEstado(EstadoJuego estado, const GestorPuntuacion& puntos, OpcionJugadores nJug) {
    char buf[32];

    // limpiamos la pantalla solo una vez cuando cambia el estado
    _pantalla->rellenarPantalla(0x0000);

    switch (estado) {
        case EstadoJuego::ST_JUEGO_INICIO:
            _pantalla->setEscala(4);
            sprintf(buf, "RONDA %d", puntos.getRondaActual());
            _pantalla->Escribir(20, 70, buf, 0xFFFF, 0x001F, 0x001F);
            // aqui no dibujamos pulsa para empezar porque lo hace la logica del juego
            break;

        case EstadoJuego::ST_TIEMPO_PREPARACION:
            _pantalla->rellenarPantalla(0x001F); // fondo azul fijo para aviso visual
            _pantalla->setEscala(3);
            _pantalla->Escribir(30, 100, "LISTO?", 0xFFFF, 0, 0);
            break;

        case EstadoJuego::ST_JUEGO_RONDA:
            _pantalla->rellenarPantalla(0xF800); // fondo rojo fijo durante la ronda
            break;

        case EstadoJuego::ST_MOSTRAR_RESULTADO_RONDA:
            _pantalla->setEscala(4);
            sprintf(buf, "%lu ms", puntos.getUltimoTiempo());
            _pantalla->Escribir(20, 80, buf, 0x07E0, 0, 0);
            // dibujamos el marcador con los puntos actuales
            dibujarMarcador(puntos, nJug);
            break;

        case EstadoJuego::ST_JUEGO_FIN:
                    _pantalla->setEscala(3);
                    // texto de fin de juego ajustado para que no se corte
                    _pantalla->Escribir(0, 50, "FIN JUEGO", 0xFFFF, 0xF800, 0);

                    // bajamos el marcador para que no choque con el titulo
                    dibujarMarcador(puntos, nJug);

                    _pantalla->setEscala(1.7);
                    // instrucciones colocadas abajo para que se lean mejor
                    _pantalla->Escribir(25, 200, "CLICK: REINTENTAR", 0x07E0, 0, 0);
                    _pantalla->Escribir(25, 220, "HOLD: SALIR", 0xF800, 0, 0);
                    break;

        default: break;
    }
}

void DibujarJuego::dibujarMarcador(const GestorPuntuacion& puntos, OpcionJugadores nJug) {
    char buf[32];
    _pantalla->setEscala(2); // escala intermedia para que se vea claro

    sprintf(buf, "J1: %lu PTS", puntos.getPuntosJ1());
    _pantalla->Escribir(20, 130, buf, 0xFFFF, 0, 0);

    if (nJug == OpcionJugadores::DOS_JUGADORES) {
        sprintf(buf, "J2: %lu PTS", puntos.getPuntosJ2());
        _pantalla->Escribir(20, 160, buf, 0xFFFF, 0, 0);
    }
}
