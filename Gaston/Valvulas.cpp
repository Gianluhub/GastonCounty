#include <Arduino.h>
#include "Pines.h"
#include "procesos.h"


// Clase para control de las valvulas
// Guarda el estado de las valvulas y lo regresa a su estado original si es necesario
class Control_Valvulas{

	int valvulas[15];
	//int motores[2];	
	const int pins[15] = {FV200,FV201,FV202,FV203,FV204,FV205,FV206,
						  FV207,FV208,FV209,FV210,FV211,FV212,FV213,FV214};
	//const int rele[2] = {pump,plegador_1};

	public:
	void guardar_estado(); // Guarda el estado actual de las valvulas y los motores
	void regresar_estado(); // Regresa el estado de las valvulas al guardado previamente
	void cerrar_valvulas(); // Cierra todas las valvulas 
};

// Funciones de la clase Control_valvulas
void Control_Valvulas::guardar_estado(){

	int i;
	for (i=0; i<15; i++)
	{
		valvulas[i] = digitalRead(pins[i]);
	}

}

void Control_Valvulas::regresar_estado(){

	int i;
	for (i=0; i<15; i++)
	{
		digitalWrite(pins[i], valvulas[i]);
	}
}

void Control_Valvulas::cerrar_valvulas(){
	int i;
	for (i=0; i<15; i++)
	{
		digitalWrite(pins[i],LOW);
	}
}



// Funciones encargadas de la interrupcion del sistema.
volatile Control_Valvulas master;
volatile int interrupt = false;

void Interrupt() {

  interrupt = true;
  master.guardar_estado();  // Guarda el estado de las valvulas si estan abiertas o cerradas
  master.cerrar_valvulas(); // Cierra todas las valvulas por seguridad
  Reset();                  // Resetea todos los contadores
}

void Detener_proceso(){

 

}
