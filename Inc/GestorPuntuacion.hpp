#ifndef GESTOR_PUNTUACION_HPP
#define GESTOR_PUNTUACION_HPP

#include <stdint.h>
#include "Definiciones.h"

class GestorPuntuacion {
public:
    GestorPuntuacion() { resetear(); }

    void resetear() {
        _puntosJ1 = 0; _puntosJ2 = 0;
        _rondaActual = 1; _ultimoTR = 0;
    }


    void registrarRonda(uint32_t tr, OpcionModo modo, uint8_t ganador) {
        _ultimoTR = tr;

        if (modo == OpcionModo::CUENTA_ATRAS) {
            // Punto fijo al que pulsa primero
            if (ganador == 1) _puntosJ1 += 100;
            else if (ganador == 2) _puntosJ2 += 100;
        }
        else if (modo == OpcionModo::MEJOR_RESULTADO) {
            uint16_t pts = calcularPuntos(tr);
            if (ganador == 1) _puntosJ1 += pts;
            else if (ganador == 2) _puntosJ2 += pts;
        }

        _rondaActual++;
    }


    uint8_t getRonda() const { return _rondaActual; }
    uint32_t getPuntosJ1() const { return _puntosJ1; }
    uint32_t getPuntosJ2() const { return _puntosJ2; }
    uint32_t getUltimoTR() const { return _ultimoTR; }
    uint8_t getRondaActual() const { return getRonda(); }
    uint32_t getUltimoTiempo() const { return getUltimoTR(); }

private:
    uint32_t _puntosJ1, _puntosJ2, _ultimoTR;
    uint8_t _rondaActual;
    OpcionJugadores _confJugadores;

    uint16_t calcularPuntos(uint32_t tr) {
        if (tr < 200) return 100;
        if (tr < 400) return 75;
        if (tr < 600) return 50;
        if (tr < 1000) return 25;
        return 10;
    }
};

#endif
