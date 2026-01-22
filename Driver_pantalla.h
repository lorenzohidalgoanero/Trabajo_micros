#ifndef DRIVER_PANTALLA_H
#define DRIVER_PANTALLA_H
#include "main.h"
class Driver_Pantalla {
private:
   SPI_HandleTypeDef* _hspi;
   uint16_t _width;
   uint16_t _height;
   // metodos privados internos del driver
   void enviarComando(uint8_t cmd);
   void enviarDatos(uint8_t* data, size_t len);
   void setVentana(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
public:
   // constructor
   Driver_Pantalla(SPI_HandleTypeDef* hspi, uint16_t ancho, uint16_t alto);
   // metodos para el usuario
   void inicializar();
   void rellenarPantalla(uint16_t color);
   // Es para dibujar píxeles gordos.
   void dibujarBloque(uint16_t x, uint16_t y, uint16_t color, uint8_t tamano);
};
#endif
