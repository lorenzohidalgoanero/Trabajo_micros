#include "Logica_Juego.h"
#include <stdlib.h>
#include <cstdio>

// Referencia a variables globales del hardware (definidas en main.c)
// Nota: "tiempo_ms" se ha dejado de utilizar para medir el tiempo de reacción. En su lugar
// se emplea HAL_GetTick(), que devuelve el número de milisegundos desde el arranque del sistema.
extern volatile uint16_t adc_ruido;

Logica_Juego::Logica_Juego(Estilo_Pantalla* pantalla)
    : _pantalla(pantalla),
      _dibujante(pantalla) {
    // Inicializamos la máquina de estados y los registros de puntuación.
    resetearJuegoCompleto();
}

void Logica_Juego::actualizar(const Entradas& inputs) {
    uint32_t Tiempo = HAL_GetTick();

    // 1. DESPERTAR / REINICIO
	if (_estadoActual == EstadoJuego::ST_JUEGO_APAGADO) {
		_puntuacion.resetear();
		_estadoActual = EstadoJuego::ST_JUEGO_INICIO;
		_redibujar = true;
		_solicitudSalidaMenu = false; // Aseguramos que empiece limpia
		return;
	}

	// HOLD GLOBAL: Reinicia el juego en cualquier ronda
	// RESTRICCIÓN: Solo actúa si NO estamos en el menú de fin de juego
	if (inputs.btn_PA0_Hold && _estadoActual != EstadoJuego::ST_JUEGO_FIN) {
		_puntuacion.resetear();
		_estadoActual = EstadoJuego::ST_JUEGO_INICIO;
		_redibujar = true;
		HAL_GPIO_WritePin(LED_AZUL_GPIO_Port, LED_AZUL_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED_ROJO_GPIO_Port, LED_ROJO_Pin, GPIO_PIN_RESET);
		return; // Salimos para no procesar el switch en este ciclo
	}
    //este hold esta fuera de los switch porque quiero que el hold me permita reiniciar el juego en cualquier ronda pero en ese ultimo menu de finjuego quiero que me saque al menu_maquina (la consola)



    // 2. MÁQUINA DE ESTADOS
    switch (_estadoActual) {

        case EstadoJuego::ST_JUEGO_INICIO:
            if (inputs.btn_PA0_Click) {
            	_solicitudSalidaMenu = false;
                _tickInicio = Tiempo;
                _estadoActual = EstadoJuego::ST_TIEMPO_PREPARACION;
                _redibujar = true; // Dispara el dibujo del fondo azul
                HAL_GPIO_WritePin(LED_AZUL_GPIO_Port, LED_AZUL_Pin, GPIO_PIN_SET);
            }
            break;

        case EstadoJuego::ST_TIEMPO_PREPARACION:
            if (Tiempo - _tickInicio >= 1500) {
                _tiempoAleatorio = obtenerTiempoEspera();
                _tickInicio = Tiempo;
                _estadoActual = EstadoJuego::ST_TIEMPO_DISPARO;
                _redibujar = true; // Fondo negro
                HAL_GPIO_WritePin(LED_AZUL_GPIO_Port, LED_AZUL_Pin, GPIO_PIN_RESET);
            }
            break;

        case EstadoJuego::ST_TIEMPO_DISPARO:
            if (Tiempo - _tickInicio >= _tiempoAleatorio) {
                _tickInicioDisparo = Tiempo;
                _estadoActual = EstadoJuego::ST_JUEGO_RONDA;
                _redibujar = true; // Fondo rojo + "YA!"
                HAL_GPIO_WritePin(LED_ROJO_GPIO_Port, LED_ROJO_Pin, GPIO_PIN_SET);
            }
            break;

        case EstadoJuego::ST_JUEGO_RONDA:
        	//Solo detecta el botón del jugador 1.
        	       	if (_confJugadores == OpcionJugadores::UN_JUGADOR ){
        	       		if (inputs.Boton_1) {
        	       			_tiempoReaccion_1 = Tiempo - _tickInicioDisparo;
        	       			_puntuacion.registrarRonda(_tiempoReaccion_1, _confModo, 1);
        	       			_tickInicio = Tiempo;
        	       			_estadoActual = EstadoJuego::ST_MOSTRAR_RESULTADO_RONDA;
        	       			_redibujar = true;
        	       			// --- NUEVO: Apagar LED y feedback sonoro corto ---
        	                       Buzzer_Beep(50); // Pitido muy corto de 50ms
               	       			HAL_GPIO_WritePin(LED_ROJO_GPIO_Port, LED_ROJO_Pin, GPIO_PIN_RESET);

        	       		}
        	       	}

        	// MODO PARA 2 JUGADORES CON MEJOR_RESULTADO
        	       	if (_confJugadores == OpcionJugadores::DOS_JUGADORES  && _confModo == OpcionModo::MEJOR_RESULTADO){
        	       		if(flag_jugador_1 == 0 || flag_jugador_2 == 0){
        	       			if (inputs.Boton_1 && flag_jugador_1 == 0) {
        	       				_tiempoReaccion_1 = Tiempo - _tickInicioDisparo;
        	       				_puntuacion.registrarRonda(_tiempoReaccion_1, _confModo, 1);
        	       				_redibujar = true;
        	       				flag_jugador_1 = 1;

        	       				// --- NUEVO: feedback sonoro corto ---
        	       	                        Buzzer_Beep(50); // Pitido muy corto de 50ms
        	       	     	 }
        	       			if (inputs.Boton_2 && flag_jugador_2 == 0) {
        	       				_tiempoReaccion_2 = Tiempo - _tickInicioDisparo;
        	       				_puntuacion.registrarRonda(_tiempoReaccion_2, _confModo, 2);
        	       				_redibujar = true;
        	       				flag_jugador_2 = 1;
        	       				// --- NUEVO: feedback sonoro corto ---
        	       	                        Buzzer_Beep(50); // Pitido muy corto de 50ms
        	       	     	 }
        	       		}
        	       		if (flag_jugador_1 == 1 && flag_jugador_2 == 1){
        	       			flag_jugador_1 = false;
        	       			flag_jugador_2 = false;
        	       			_tickInicio = Tiempo;
        	       			_estadoActual = EstadoJuego::ST_MOSTRAR_RESULTADO_RONDA;
        	       			HAL_GPIO_WritePin(LED_ROJO_GPIO_Port, LED_ROJO_Pin, GPIO_PIN_RESET);
        	       		}
        	       	 }

        	       	// MODO PARA 2 JUGADORES CON CUENTA_ATRAS
        	       	if (_confJugadores == OpcionJugadores::DOS_JUGADORES  && _confModo == OpcionModo::CUENTA_ATRAS){
        	       			if (inputs.Boton_1 ) {
        	       				_tiempoReaccion_1 = Tiempo - _tickInicioDisparo;
        	       				_puntuacion.registrarRonda(_tiempoReaccion_1, _confModo, 1);
        	       				_redibujar = true;
            	       			_tickInicio = Tiempo;
            	       			_estadoActual = EstadoJuego::ST_MOSTRAR_RESULTADO_RONDA;
            	       			HAL_GPIO_WritePin(LED_ROJO_GPIO_Port, LED_ROJO_Pin, GPIO_PIN_RESET);
        	       				// --- NUEVO: feedback sonoro corto ---
        	       	                        Buzzer_Beep(50); // Pitido muy corto de 50ms
        	       	     	 }
        	       			else if (inputs.Boton_2 ) {
        	       				_tiempoReaccion_2 = Tiempo - _tickInicioDisparo;
        	       				_puntuacion.registrarRonda(_tiempoReaccion_2, _confModo, 2);
        	       				_redibujar = true;
            	       			_tickInicio = Tiempo;
            	       			_estadoActual = EstadoJuego::ST_MOSTRAR_RESULTADO_RONDA;
            	       			HAL_GPIO_WritePin(LED_ROJO_GPIO_Port, LED_ROJO_Pin, GPIO_PIN_RESET);

        	       				// --- NUEVO: feedback sonoro corto ---
        	       	                        Buzzer_Beep(50); // Pitido muy corto de 50ms
        	       	     	 }

        	       	 }
        	       	break;



        case EstadoJuego::ST_MOSTRAR_RESULTADO_RONDA:
            // Aumentamos a 3 segundos para asegurar que el usuario vea el tiempo
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

    // 3. LLAMADA CRÍTICA A VISUALIZAR
    visualizar();
}

uint32_t Logica_Juego::obtenerTiempoEspera() {
    // Implementación basada en la dificultad seleccionada
    uint32_t div_dif = (_confDificultad == OpcionDificultad::DIFICIL) ? 2 : 1;
    // Genera un tiempo aleatorio entre 1s y 3s (ajustado por dificultad)
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
    _tiempoReaccion_1 = 0;
    _tiempoReaccion_2 = 0;

    HAL_GPIO_WritePin(LED_AZUL_GPIO_Port, LED_AZUL_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LED_ROJO_GPIO_Port, LED_ROJO_Pin, GPIO_PIN_RESET);
}


void Logica_Juego::Buzzer_Beep(uint16_t duracion) {
    HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET);
    HAL_Delay(duracion);
    HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
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
    // Aquí podríamos inicializar contadores o recursos para una nueva ronda.
    // Por ahora se gestiona en actualizar() al pasar a EstadoJuego::ACCION.
}
