#include "Logica_Juego.h"
#include <stdlib.h> 
#include <stdio.h>  

// CODIGO EJEMPLO PARA LUEGO EL MERGE, NO ES PRODUCTO FINAL DEL JUEGO


Logica_Juego::Logica_Juego(Estilo_Pantalla* pantalla)
   : _pantalla(pantalla) {
   resetearJuegoCompleto();
}
void Logica_Juego::configurar(OpcionJugadores jug, OpcionModo modo, OpcionDificultad dif) {
   _confJugadores = jug;
   _confModo = modo;
   _confDificultad = dif;
}
void Logica_Juego::resetearJuegoCompleto() {
   _estadoActual = EstadoJuego::INIT_LOCAL;
   _redibujar = true;
   _tickInicio = 0;
   _tiempoReaccion = 0;
}
uint32_t Logica_Juego::obtenerTiempoEspera() {
   uint32_t base = 2000;
   if (_confDificultad == OpcionDificultad::DIFICIL) base = 500;
   else if (_confDificultad == OpcionDificultad::FACIL) base = 4000;
   return base + (HAL_GetTick() % 2000);
}
void Logica_Juego::actualizar(const Entradas& inputs) {
   uint32_t ahora = HAL_GetTick();
   // Nota: Como en Main.c distinguimos Click de Hold,
   // inputs.btn_PA0_Click SOLO será true si fue una pulsación CORTA.
   switch (_estadoActual) {
       case EstadoJuego::INIT_LOCAL:
           if (_redibujar) {
               _pantalla->rellenarPantalla(0x0000);
               _pantalla->Escribir(40, 100, "LISTO?", 0xFFFF, 0, 0);
               _redibujar = false;
           }
           // Click corto para empezar
           if (inputs.btn_PA0_Click) {
               _estadoActual = EstadoJuego::ESPERANDO_AZAR;
               _tiempoAleatorio = obtenerTiempoEspera();
               _tickInicio = ahora;
               _redibujar = true;
           }
           break;
       case EstadoJuego::ESPERANDO_AZAR:
            if (_redibujar) {
               _pantalla->rellenarPantalla(0x0000);
               _pantalla->Escribir(80, 100, "...", 0xFFFF, 0, 0);
               _redibujar = false;
           }
           // TRAMPA: Si hace click antes de tiempo
           if (inputs.btn_PA0_Click) {
                _pantalla->Escribir(20, 40, "FALSO!", 0xF800, 0, 0);
                HAL_Delay(1000);
                resetearJuegoCompleto();
                return;
           }
           if ((ahora - _tickInicio) >= _tiempoAleatorio) {
               _estadoActual = EstadoJuego::ACCION;
               _tickInicio = ahora;
               _redibujar = true;
           }
           break;
       case EstadoJuego::ACCION:
           if (_redibujar) {
               _pantalla->rellenarPantalla(0x07E0); // VERDE
               _redibujar = false;
           }
           // REACCIÓN VÁLIDA
           if (inputs.btn_PA0_Click) {
               _tiempoReaccion = ahora - _tickInicio;
               _estadoActual = EstadoJuego::RESULTADO;
               _redibujar = true;
           }
           break;
       case EstadoJuego::RESULTADO:
           if (_redibujar) {
                _pantalla->rellenarPantalla(0x0000);
                char buf[20];
                sprintf(buf, "%lu ms", _tiempoReaccion);
                _pantalla->Escribir(20, 100, buf, 0xFFFF, 0, 0);
                _pantalla->Escribir(10, 200, "Click: Otra vez", 0xAAAA, 0,0);
                _pantalla->Escribir(10, 220, "Hold: Salir", 0xAAAA, 0,0);
                _redibujar = false;
           }
           if (inputs.btn_PA0_Click) {
               resetearJuegoCompleto();
           }
           break;
       case EstadoJuego::GAMEOVER:
           break;
   }
}
