#include "Maquina_Menu.h"
#include  "ESTILO_PANTALLA.h"
#include  "DRIVER_PANTALLA.h"
// colores PARA EL EFECTO 3D RETRO
#define COLOR_GLITCH_FRENTE 0xFFE0
#define COLOR_GLITCH_MEDIO  0xF800
#define COLOR_GLITCH_FONDO  0x001F
Maquina_Menu::Maquina_Menu(Estilo_Pantalla* pantalla, Logica_Juego* juego)
   : _pantalla(pantalla), _juego(juego), _estadoActual(EstadoMenu::APAGADO)
{
   _selJugadores = OpcionJugadores::UN_JUGADOR;
   _selModo = OpcionModo::CUENTA_ATRAS;
   _selDificultad = OpcionDificultad::MEDIO;
   _tickParpadeo = 0;
   _textoVisible = true;
   _inicioDibujado = false;
}
//							    	>>>>>>>>>>>>>>  MAQUINA DE ESTADOS  <<<<<<<<<<<<<<<<<<<<<
void Maquina_Menu::actualizar(const Entradas& inputs) { //estos inputs estan en definiciones
   switch (_estadoActual) { //los estados construyen la pantalla y variables del siguiente estado si se pulsa el boton
       case EstadoMenu::APAGADO:
           dibujarPantallaInicio();
           //si se pulsa el boton cambiamos de estado
           if (inputs.btn_PA0_Click || inputs.btn_PA0_Hold) {
               _selJugadores = OpcionJugadores::UN_JUGADOR;
               _estadoActual = EstadoMenu::SEL_JUGADORES;
               _pantalla->rellenarPantalla(0x0005);
               dibujarMenuJugadores();
           }
           break;
       case EstadoMenu::SEL_JUGADORES:
           if (inputs.btn_PA0_Click) {
               // logica de alternancia simple
               if (_selJugadores == OpcionJugadores::UN_JUGADOR)
                   _selJugadores = OpcionJugadores::DOS_JUGADORES;
               else
                   _selJugadores = OpcionJugadores::UN_JUGADOR;
               dibujarMenuJugadores();
           }
           if (inputs.btn_PA0_Hold) { //si mantenemos el boton cambiamos de estado
               _selModo = OpcionModo::CUENTA_ATRAS; // dejamos seleccionado la primera opcion
               _estadoActual = EstadoMenu::SEL_MODO;
               _pantalla->rellenarPantalla(0x0005);
               dibujarMenuModo();
           }
           break;
       case EstadoMenu::SEL_MODO:
           if (inputs.btn_PA0_Click) { //si le damos click cambiamos de seleccion
               if (_selModo == OpcionModo::CUENTA_ATRAS)
                   _selModo = OpcionModo::MEJOR_RESULTADO;
               else
                   _selModo = OpcionModo::CUENTA_ATRAS;
               dibujarMenuModo();
           }
           if (inputs.btn_PA0_Hold) { // si holdeamos se queda guardada la configuracion anterior del selector
               _selDificultad = OpcionDificultad::FACIL; // dejamos el nivel ya seleccionado
               _estadoActual = EstadoMenu::SEL_DIFICULTAD;//cambiamos de estado
               _pantalla->rellenarPantalla(0x0005);
               dibujarMenuDificultad();
           }
           break;
       case EstadoMenu::SEL_DIFICULTAD:
           if (inputs.btn_PA0_Click) {
               if (_selDificultad == OpcionDificultad::FACIL)
                   _selDificultad = OpcionDificultad::MEDIO;
               else if (_selDificultad == OpcionDificultad::MEDIO)
                   _selDificultad = OpcionDificultad::DIFICIL;
               else
                   _selDificultad = OpcionDificultad::FACIL;
               dibujarMenuDificultad();
           }
           if (inputs.btn_PA0_Hold) {
               _estadoActual = EstadoMenu::PANTALLA_CARGA;
               _pantalla->rellenarPantalla(0x0005);
               dibujarCarga(); //					PANTALLA DE CARGA JUEGO
               _juego->configurar(_selJugadores, _selModo, _selDificultad); // configuramos el juego dentro de SU MAQUINA DE ESTADOS local
               _juego->resetearJuegoCompleto();
           }
           break;
       case EstadoMenu::PANTALLA_CARGA:
           if (inputs.btn_PA0_Hold || inputs.btn_PA0_Click) { //INICIAMOS EL JUEGO COMO TAL
               _estadoActual = EstadoMenu::JUGANDO;
           }
           break;
       case EstadoMenu::JUGANDO: // 						ESTO HAY QUE CAMBIARLO, mantener el boton es lo que te saca del juego
           if (inputs.btn_PA0_Hold) {
               _estadoActual = EstadoMenu::APAGADO;
               _juego->resetearJuegoCompleto();
               _pantalla->rellenarPantalla(0x0000);
               _inicioDibujado = false;
           } else {
               _juego->actualizar(inputs);
           }
           break;
   }
}
//							  	>>>>>>>>>>>>>>  FUNCIONES MENUS SEGÚN ESTADO  <<<<<<<<<<<<<<<<<<<<<
void Maquina_Menu::dibujarPantallaInicio() {
   if (!_inicioDibujado) {
       _pantalla->rellenarPantalla(0x0000);
       // TITULO GRANDE (Escala 3)
       _pantalla->setEscala(3);
       // centrado aprox para escala 3 (letras de 24px ancho)
       int x = 12;
       int y = 80;
       _pantalla->Escribir(x+3, y+3, "REFLEJOS", COLOR_GLITCH_FONDO, 0, 0);
       _pantalla->Escribir(x-3, y-3, "REFLEJOS", COLOR_GLITCH_MEDIO, 0, 0);
       _pantalla->Escribir(x, y,   "REFLEJOS", COLOR_GLITCH_FRENTE, 0, 0);
       _inicioDibujado = true;
       _tickParpadeo = HAL_GetTick();
   }
   // TEXTO PEQUEÑO PARPADEANTE
   if ((HAL_GetTick() - _tickParpadeo) > 500) {
       _tickParpadeo = HAL_GetTick();
       _textoVisible = !_textoVisible;
       _pantalla->setEscala(1); // LETRA PEQUEÑA
       if (_textoVisible)
           _pantalla->Escribir(44, 180, "Pulsa para comenzar", 0xFFFF, 0, 0);
       else
           _pantalla->Escribir(44, 180, "Pulsa para comenzar", 0x0000, 0, 0);
       _pantalla->setEscala(2); // Restaurar escala
   }
}
void Maquina_Menu::dibujarMenuJugadores() {
   _pantalla->setEscala(2);
   // Colores y Flechas
   uint16_t c1 = (_selJugadores == OpcionJugadores::UN_JUGADOR) ? 0xFFE0 : 0xFFFF;
   uint16_t c2 = (_selJugadores == OpcionJugadores::DOS_JUGADORES) ? 0xFFE0 : 0xFFFF;
   const char* f1 = (_selJugadores == OpcionJugadores::UN_JUGADOR) ? ">" : " ";
   const char* f2 = (_selJugadores == OpcionJugadores::DOS_JUGADORES) ? ">" : " ";
   _pantalla->Escribir(20, 20, "JUGADORES", 0xFFFF, 0, 0);
   _pantalla->Escribir(20, 60, f1, c1, 0, 0);
   _pantalla->Escribir(40, 60, "1 JUGADOR", c1, 0, 0);
   _pantalla->Escribir(20, 90, f2, c2, 0, 0);
   _pantalla->Escribir(40, 90, "2 JUGADORES", c2, 0, 0);
   _pantalla->setEscala(1);
   _pantalla->Escribir(10, 210, "Click:Cambiar Hold:Ok", 0x07E0, 0,0);
}
void Maquina_Menu::dibujarMenuModo() {
   _pantalla->setEscala(2);
   uint16_t c1 = (_selModo == OpcionModo::CUENTA_ATRAS)    ? 0xFFE0 : 0xFFFF;
   uint16_t c2 = (_selModo == OpcionModo::MEJOR_RESULTADO) ? 0xFFE0 : 0xFFFF;
   const char* f1 = (_selModo == OpcionModo::CUENTA_ATRAS) ? ">" : " ";
   const char* f2 = (_selModo == OpcionModo::MEJOR_RESULTADO) ? ">" : " ";
   _pantalla->Escribir(20, 20, "MODO JUEGO", 0xFFFF, 0, 0);
   _pantalla->Escribir(10, 60, f1, c1, 0, 0);
   _pantalla->Escribir(30, 60, "CUENTA ATRAS", c1, 0, 0);
   _pantalla->Escribir(10, 90, f2, c2, 0, 0);
   _pantalla->Escribir(30, 90, "MEJOR TIEMPO", c2, 0, 0);
   _pantalla->setEscala(1);
   _pantalla->Escribir(10, 210, "Click:Cambiar Hold:Ok", 0x07E0, 0,0);
}
void Maquina_Menu::dibujarMenuDificultad() {
   _pantalla->setEscala(2);
   uint16_t c1 = (_selDificultad == OpcionDificultad::FACIL)   ? 0xFFE0 : 0xFFFF;
   uint16_t c2 = (_selDificultad == OpcionDificultad::MEDIO)   ? 0xFFE0 : 0xFFFF;
   uint16_t c3 = (_selDificultad == OpcionDificultad::DIFICIL) ? 0xFFE0 : 0xFFFF;
   const char* f1 = (_selDificultad == OpcionDificultad::FACIL) ? ">" : " ";
   const char* f2 = (_selDificultad == OpcionDificultad::MEDIO) ? ">" : " ";
   const char* f3 = (_selDificultad == OpcionDificultad::DIFICIL) ? ">" : " ";
   _pantalla->Escribir(20, 20, "DIFICULTAD", 0xFFFF, 0, 0);
   _pantalla->Escribir(20, 60, f1, c1, 0, 0);
   _pantalla->Escribir(40, 60, "FACIL", c1, 0, 0);
   _pantalla->Escribir(20, 90, f2, c2, 0, 0);
   _pantalla->Escribir(40, 90, "MEDIO", c2, 0, 0);
   _pantalla->Escribir(20, 120, f3, c3, 0, 0);
   _pantalla->Escribir(40, 120,"DIFICIL", c3, 0, 0);
   _pantalla->setEscala(1);
   _pantalla->Escribir(10, 210, "Click:Cambiar Hold:Ok", 0x07E0, 0,0);
}
void Maquina_Menu::dibujarCarga() {
   _pantalla->setEscala(2);
   _pantalla->Escribir(40, 60, "LISTO?", 0xFFFF, 0, 0);
   _pantalla->Escribir(30, 110, "> EMPEZAR <", 0x07E0, 0xF800, 0);
   _pantalla->setEscala(1);
   _pantalla->Escribir(50, 200, "HOLD TO START", 0xFFFF, 0, 0);
}
