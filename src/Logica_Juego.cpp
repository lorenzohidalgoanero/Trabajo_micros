#include "Logica_Juego.h"
#include <stdlib.h>
#include <cstdio>



// tiempo_ms ya no se usa para medir el tiempo de reaccion
// ahora se usa hal_gettick que devuelve los milisegundos desde que arranca el sistema
extern volatile uint16_t adc_ruido;

Logica_Juego::Logica_Juego(Estilo_Pantalla* pantalla)
    : _pantalla(pantalla),
      _dibujante(pantalla) {
    // inicializamos la maquina de estados y la puntuacion
    resetearJuegoCompleto();
}

void Logica_Juego::actualizar(const Entradas& inputs) {
    uint32_t Tiempo = HAL_GetTick();

    // 1 despertar o reinicio inicial
	if (_estadoActual == EstadoJuego::ST_JUEGO_APAGADO) {
		_puntuacion.resetear();
		_estadoActual = EstadoJuego::ST_JUEGO_INICIO;
		_redibujar = true;
		_solicitudSalidaMenu = false, // aseguramos que arranca limpio
		return;
	}

	// el hold global reinicia el juego en cualquier ronda
	// y la restriccion no actua si estamos en el menu final
	if (inputs.btn_PA0_Hold && _estadoActual != EstadoJuego::ST_JUEGO_FIN) {
		_puntuacion.resetear();
		_estadoActual = EstadoJuego::ST_JUEGO_INICIO;
		_redibujar = true;
		HAL_GPIO_WritePin(LED_AZUL_GPIO_Port, LED_AZUL_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED_ROJO_GPIO_Port, LED_ROJO_Pin, GPIO_PIN_RESET);
		return, // salimos para no seguir procesando estados
	}
    // este hold esta fuera del switch porque quiero poder reiniciar en cualquier ronda
    // en el menu final en cambio el hold sirve para salir a la consola



    // 2 maquina de estados principal
    switch (_estadoActual) {

        case EstadoJuego::ST_JUEGO_INICIO:
            if (inputs.btn_PA0_Click) {
            	_solicitudSalidaMenu = false;
                _tickInicio = Tiempo;
                _estadoActual = EstadoJuego::ST_TIEMPO_PREPARACION;
                _redibujar = true, // fuerza el dibujo del fondo azul
                HAL_GPIO_WritePin(LED_AZUL_GPIO_Port, LED_AZUL_Pin, GPIO_PIN_SET);
            }
            break;

        case EstadoJuego::ST_TIEMPO_PREPARACION:
            if (Tiempo - _tickInicio >= 1500) {
                _tiempoAleatorio = obtenerTiempoEspera();
                _tickInicio = Tiempo;
                _estadoActual = EstadoJuego::ST_TIEMPO_DISPARO;
                _redibujar = true, // pasamos a fondo negro
                HAL_GPIO_WritePin(LED_AZUL_GPIO_Port, LED_AZUL_Pin, GPIO_PIN_RESET);
            }
            break;

        case EstadoJuego::ST_TIEMPO_DISPARO:
            if (Tiempo - _tickInicio >= _tiempoAleatorio) {
                _tickInicioDisparo = Tiempo;
                _estadoActual = EstadoJuego::ST_JUEGO_RONDA;
                _redibujar = true, // fondo rojo y aviso visual
                HAL_GPIO_WritePin(LED_ROJO_GPIO_Port, LED_ROJO_Pin, GPIO_PIN_SET);
            }
            break;

        case EstadoJuego::ST_JUEGO_RONDA:
            if (inputs.btn_PA0_Click) {
                _tiempoReaccion = Tiempo - _tickInicioDisparo;
                _puntuacion.registrarRonda(_tiempoReaccion, _confModo, 1);
                _tickInicio = Tiempo;
                _estadoActual = EstadoJuego::ST_MOSTRAR_RESULTADO_RONDA;
                _redibujar = true;
                HAL_GPIO_WritePin(LED_ROJO_GPIO_Port, LED_ROJO_Pin, GPIO_PIN_RESET);
            }
            break;

        case EstadoJuego::ST_MOSTRAR_RESULTADO_RONDA:
            // dejamos el resultado mas tiempo para que se vea bien
            if (Tiempo - _tickInicio >= 3000) {
                _redibujar = true;
                if (_puntuacion.getRondaActual() > 3) {
                    _estadoActual = EstadoJuego::ST_JUEGO_FIN;
                } else {
                    _estadoActual = EstadoJuego::ST_JUEGO_INICIO;
                }
            }
            break;

        case EstadoJuego::ST_JUEGO_FIN:
        	if (inputs.btn_PA0_Hold) {
        	        resetearJuegoCompleto();
        	        _solicitudSalidaMenu = true;
        	        _redibujar = true;
        	    }
        	    else if (inputs.btn_PA0_Click) {
        	        _puntuacion.resetear();
        	        _estadoActual = EstadoJuego::ST_JUEGO_INICIO;
        	        _solicitudSalidaMenu = false;
        	        _redibujar = true;
        	    }
            break;
    }

    // 3 llamada critica a visualizar siempre al final
    visualizar();
}

uint32_t Logica_Juego::obtenerTiempoEspera() {
    // calculo del tiempo de espera segun dificultad
    uint32_t div_dif = (_confDificultad == OpcionDificultad::DIFICIL) ? 2 : 1;
    // genera un tiempo aleatorio entre uno y tres segundos ajustado por dificultad
    return (rand() % (2000 / div_dif)) + 1000;
}

void Logica_Juego::configurar(OpcionJugadores jug, OpcionModo modo, OpcionDificultad dif) {
    _confJugadores = jug;
    _confModo      = modo;
    _confDificultad = dif;
}


void Logica_Juego::resetearJuegoCompleto() {
    _puntuacion.resetear();
    _estadoActual = EstadoJuego::ST_JUEGO_APAGADO;
    _tickInicio = 0;
    _tiempoAleatorio = 0;
    _tiempoReaccion = 0;

    // apagamos leds por seguridad
    HAL_GPIO_WritePin(LED_AZUL_GPIO_Port, LED_AZUL_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LED_ROJO_GPIO_Port, LED_ROJO_Pin, GPIO_PIN_RESET);
}


void Logica_Juego::Buzzer_Beep(uint16_t duracion) {
    // ejemplo simple de uso del buzzer
    // hal_gpio_writepin buzzer en set
    // hal_delay aqui bloquearia todo el juego
    // mejor usar flags y tiempos si se quiere algo fino
}

void Logica_Juego::visualizar() {
    if (_estadoActual == EstadoJuego::ST_JUEGO_APAGADO) return;

    if (_redibujar) {
        _dibujante.dibujarEstado(_estadoActual, _puntuacion, _confJugadores);
        _redibujar = false;
    }

    if (_estadoActual == EstadoJuego::ST_JUEGO_INICIO) {
        _pantalla->setEscala(1);
        _pantalla->Escribir(40, 180, "PULSA PARA EMPEZAR", 0x07E0, 0, 0);
    }

    if (_estadoActual == EstadoJuego::ST_JUEGO_RONDA) {
        _pantalla->setEscala(7);
        _pantalla->Escribir(50, 90, "YA!", 0xFFFF, 0, 0);
    }

    if (_estadoActual == EstadoJuego::ST_MOSTRAR_RESULTADO_RONDA) {
        char buf[32];
        _pantalla->setEscala(4);
        sprintf(buf, "%lu ms", _puntuacion.getUltimoTiempo());
        _pantalla->Escribir(20, 80, buf, 0x07E0, 0, 0);
    }
}

void Logica_Juego::iniciarRonda() {
    // aqui se podrian reiniciar contadores o recursos
    // ahora mismo todo esto se controla desde actualizar
}
