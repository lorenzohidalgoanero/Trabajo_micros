#include "Driver_Pantalla.h"

//De cara a no tener to.do en el main se ha creado este código.
// Es un "DRIVER" que gestiona to.do lo que es la comunicación y control físico
// del TFT ST7789, la pantalla OLED que hemos implementado.


//  DC -> PB1, RES -> PB0
#define DC_CMD()  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET)
#define DC_DATA() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET)
#define RST_LOW() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET)
#define RST_HIGH() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET)

// --- Comandos ST7789 ---
#define ST7789_SWRESET 0x01 // Reinicia el chip (soft-reset).
#define ST7789_SLPOUT  0x11 // Saca a la pantalla del modo sueño para que pueda trabajar.
#define ST7789_COLMOD  0x3A // Calidad de color: decide cuántos bits usas por píxel (ej. 16-bit color).
#define ST7789_MADCTL  0x36 // Controla la rotación (horizontal/vertical) y el orden de colores (RGB vs BGR).
#define ST7789_CASET   0x2A // Dónde pintar (ancho): define desde qué columna hasta cuál vamos a dibujar.
#define ST7789_RASET   0x2B // Dónde pintar (alto): define desde qué fila hasta cuál vamos a dibujar.
#define ST7789_RAMWR   0x2C // Prepara la memoria para recibir los datos de los píxeles que vas a mandar.
#define ST7789_DISPON  0x29 // Enciende el panel para que muestre la imagen (sino se queda negra).
#define ST7789_INVON   0x21 // Invierte los colores (lo blanco se ve negro y viceversa).
#define ST7789_NORON   0x13 // Quita efectos raros o zoom y usa la pantalla completa estándar.

Driver_Pantalla::Driver_Pantalla(SPI_HandleTypeDef* hspi, uint16_t ancho, uint16_t alto)
    : _hspi(hspi), _width(ancho), _height(alto) {}

void Driver_Pantalla::enviarComando(uint8_t cmd) {
    DC_CMD();
    HAL_SPI_Transmit(_hspi, &cmd, 1, HAL_MAX_DELAY);
}

//  cada píxel de la pantalla necesita 2 bytes de información para saber de qué color pintarse.
void Driver_Pantalla::enviarDatos(uint8_t* data, size_t len) {
    DC_DATA();
    HAL_SPI_Transmit(_hspi, data, len, HAL_MAX_DELAY);
}

void Driver_Pantalla::inicializar() {
    RST_LOW(); HAL_Delay(20);
    RST_HIGH(); HAL_Delay(120);

    enviarComando(ST7789_SWRESET); HAL_Delay(150);
    enviarComando(ST7789_SLPOUT);  HAL_Delay(120);

    // La lógica es:  (1)(Comando) -> (2)(Dato)
    // PASO 1: Avisamos de QUÉ vamos a configurar
    // PASO 2: Enviamos el valor de esa configuración

    enviarComando(ST7789_COLMOD);
    uint8_t colorMode = 0x55; // trabajar en colores 16-bit
    enviarDatos(&colorMode, 1);

    enviarComando(ST7789_MADCTL);
    uint8_t madctl = 0x00;
    enviarDatos(&madctl, 1);

    enviarComando(ST7789_INVON);
    enviarComando(ST7789_NORON);
    enviarComando(ST7789_DISPON);
    HAL_Delay(120);
}

void Driver_Pantalla::setVentana(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    uint8_t data[4];

    // CONSTRUIMOS LA MATRIZ SOBRE LA QUE TRABAJAREMOS con la TFT st7789

    // definicion de las columnas, eje X
	enviarComando(ST7789_CASET);
	// Como x0 es de 16 bits (ej. 240), y SPI envía de 8 en 8, hay que partirlo:
	data[0] = x0 >> 8;   // byte alto, los 8 bits de la izquierda
	data[1] = x0 & 0xFF; // byte bajo, los 8 bits de la derecha
	data[2] = x1 >> 8;   // fin de X, byte alto
	data[3] = x1 & 0xFF; // fin de X, byte bajo
	enviarDatos(data, 4);

	// definimos las filas, eje Y
	enviarComando(ST7789_RASET);
	data[0] = y0 >> 8; data[1] = y0 & 0xFF; // inicio Y
	data[2] = y1 >> 8; data[3] = y1 & 0xFF; // fin Y
	enviarDatos(data, 4);

    enviarComando(ST7789_RAMWR);
}


//DIBUJAR LA PANTALLA

//Recuerda que cada píxel de la pantalla necesita 2 bytes de información para saber de qué color pintarse.
// USAMOS 16 bits para mostrar lo que deberian ser 24:
// 16 bits que son 5 bits Rojo, 6 bits Verde, 5 bits Azul en  RGB565
// en lugar de 24 bits (RGB888): 8 bits Rojo, 8 bits Verde, 8 bits Azul


void Driver_Pantalla::rellenarPantalla(uint16_t color) {
    setVentana(0, 0, _width - 1, _height - 1);

    //LA VARIABLE COLOR YA ESTABA DEFINIDA ANTES, solo divido en 2 conjuntos de 8 bits el mensaje que envio
    uint8_t hi = color >> 8;
    uint8_t lo = color & 0xFF;

    // preparamos un paquete (buffer) donde caben 64 bytes. Como cada píxel ocupa 2 bytes, caben 32 píxeles
    uint8_t buffer[64];
    // llenamos el paquete de (Hi, Lo), (Hi, Lo), (Hi, Lo)...
    for(int i=0; i<64; i+=2) { buffer[i] = hi; buffer[i+1] = lo; }

    DC_DATA();
    uint32_t pixels = _width * _height;
    // Enviamos el mismo buffer (pq es el mismo color siempre) en bloques para no saturar la RAM
    for(uint32_t i = 0; i < pixels; i += 32) {
         HAL_SPI_Transmit(_hspi, buffer, 64, HAL_MAX_DELAY);
    }
}

void Driver_Pantalla::dibujarBloque(uint16_t x, uint16_t y, uint16_t color, uint8_t tamano) {
    if((x + tamano) > _width || (y + tamano) > _height) return;

    setVentana(x, y, x + tamano - 1, y + tamano - 1);

    //LA VARIABLE COLOR YA ESTABA DEFINIDA ANTES, solo divido en 2 conjuntos de 8 bits el mensaje que envio
    uint8_t hi = color >> 8;
    uint8_t lo = color & 0xFF;

    // Buffer dinámico según el tamaño del bloque (letra escalada)
    int totalBytes = tamano * tamano * 2;
    uint8_t tempBuf[512]; // Buffer estático para evitar malloc/free constante

    for(int i=0; i<totalBytes && i<512; i+=2){
        tempBuf[i] = hi;
        tempBuf[i+1] = lo;
    }

    DC_DATA();
    HAL_SPI_Transmit(_hspi, tempBuf, totalBytes, HAL_MAX_DELAY);
}
