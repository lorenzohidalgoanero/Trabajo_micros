#ifndef DEFINICIONES_H
#define DEFINICIONES_H

#include <stdint.h>
#include <stdbool.h>

// Estados de la CONSOLA (Menú Principal)
enum class EstadoMenu {
    APAGADO= 0,
    SEL_JUGADORES,  // 1 o 2
    SEL_MODO,       // Cuenta atrás vs Mejor resultado
    SEL_DIFICULTAD, // Facil, Medio, Dificil
    PANTALLA_CARGA, // "Empezar"
    JUGANDO         // Aquí corre la lógica del juego
};

// Opciones seleccionables
enum class OpcionJugadores { UN_JUGADOR,FANTASMA_1, DOS_JUGADORES,FANTASMA_2 };
enum class OpcionModo      { CUENTA_ATRAS,FANTASMA_3, MEJOR_RESULTADO,FANTASMA_4 };
enum class OpcionDificultad{ FACIL,FANTASMA_5, MEDIO,FANTASMA_6, DIFICIL,FANTASMA_7 };

// Estructura para pasarle los botones a la lógica sin ensuciar con HAL_GPIO
struct Entradas {
    bool btn_PA0_Click;   // Click simple (Confirmar / Encender)
    bool btn_PA0_Hold;    // Mantener (Para encendido inicial si quieres)
    bool btn_Atras;       // Volver al menú anterior
    bool btn_Salir;       // Reset global (vuelve a APAGADO)
    bool btn_ReiniciarJuego; // Reinicia solo el juego (estando dentro)
    bool Boton_1;   // Click J1
    bool Boton_2;   // Click J2
    // Joystick (simulado por ahora como flags)
    bool joy_Arriba;
    bool joy_Abajo;
};


enum class EstadoJuego {
    ST_JUEGO_APAGADO,
    ST_JUEGO_INICIO,
    ST_TIEMPO_PREPARACION,
    ST_TIEMPO_DISPARO,
    ST_JUEGO_RONDA,
    ST_MOSTRAR_RESULTADO_RONDA,
    ST_JUEGO_FIN
};

#endif
