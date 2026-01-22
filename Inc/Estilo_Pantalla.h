#ifndef ESTILO_PANTALLA_H
#define ESTILO_PANTALLA_H

#include "DRIVER_PANTALLA.h" // Incluimos el driver de hardware que ya tienes
#include <string.h>

class Estilo_Pantalla {
private:
    Driver_Pantalla* _tft;  // Puntero al driver de la pantalla
    uint8_t _escala;        // Tamaño de la letra

    // Metodo privado auxiliar para dibujar un caracter simple sin efectos
    void dibujarLetraBase(int x, int y, char c, uint16_t color);

public:
    Estilo_Pantalla(Driver_Pantalla* tft, uint8_t escala);

    void rellenarPantalla(uint16_t color);

    void setEscala(uint8_t nuevaEscala);

    void Escribir(int x, int y, const char* texto,
                  uint16_t colorFrente,
                  uint16_t colorSombraDer,
                  uint16_t colorSombraAbajo);
};
#endif
