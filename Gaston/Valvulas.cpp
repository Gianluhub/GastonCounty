#include <Arduino.h>
#include "Pines.h"
#include "procesos.h"

// Clase para control de las valvulas
// Guarda el estado de las valvulas y lo regresa a su estado original si es necesario
class Control_Valvulas{

  int valvulas[14];
  int motores[3]; 
  const int pins[14] = {FV200,FV201,FV202,FV203,FV205,FV206,
              FV207,FV208,FV209,FV210,FV211,FV212,FV213,FV214};
  //const int rele[3] = {pump,plegador_1,jet_1};

  public:
  void guardar_estado(); // Guarda el estado actual de las valvulas y los motores
  void regresar_estado(); // Regresa el estado de las valvulas al guardado previamente
  void cerrar_valvulas(); // Cierra todas las valvulas y apaga los motores
  
};  

// Funciones encargadas de la interrupcion del sistema.
Control_Valvulas master;
extern volatile int interrupt;
extern volatile int start;
extern int Intrr;
extern int Intrr_2;
extern int Nuevo_estado_ok;
extern int nProc;
extern int nPaso;
extern int Mostrar;						// Se utiliza para volver a enviar los datos del proceso en pantalla en caso de que ocurra una interrupcion

void Interrupt() {
	interrupt = true;
	start = false;            
}

void Detener_proceso(){
  Serial.println(nProc);
  master.guardar_estado();  // Guarda el estado de las valvulas si estan abiertas o cerradas
	master.cerrar_valvulas() // Cierra todas las valvulas por seguridad
	Reset();									// Reinicia todos los temporizadores
	Mostrar = true;

}

void Reiniciar_proceso(){

	master.regresar_estado();
	start = true;
	interrupt = false;
}

void Callback_ISR(){
	static int state = 1;
	
	switch(state)
	{
		case 1:
			send_Strmsj("page TomaMuestra");
			Seleccion_proceso(nProc,nPaso);
			if(Detener_proceso()) state=2;
		break;

		case 2:
			if(digitalRead(START) >= HIGH && !Nuevo_estado_ok)
			{
				send_Strmsj("page Proceso");
				Reiniciar_proceso();
				state = 1;

			}else if(digitalRead(START) >= HIGH && Nuevo_estado_ok)
			{	
				
				if(nProc == 7 || nProc == 8) Intrr_2 = true;
				else Intrr = true;
				
				start = true;
				Nuevo_estado_ok = false;
				interrupt = false;
				state = 1;
			}	
		break;
	}

}
// Funciones de la clase Control_valvulas
void Control_Valvulas::guardar_estado(){

	int i;
	for (i=0; i<14; i++)
	{
		valvulas[i] = digitalRead(pins[i]);
	}

		motores[0] = digitalRead(pump);
		motores[1] = digitalRead(plegador_1);
		motores[2] = digitalRead(jet_1);
		Serial.println(motores[0]);
}

void Control_Valvulas::regresar_estado(){

	int i;
	for (i=0; i<14; i++)
	{
		digitalWrite(pins[i], valvulas[i]);
	}

	digitalWrite(pump,motores[0]); 	
  digitalWrite(plegador_1,motores[1]);
  digitalWrite(jet_1,motores[2]);


}

void Control_Valvulas::cerrar_valvulas(){
	int i;
	for (i=0; i<14; i++)
	{
		digitalWrite(pins[i],LOW);
	}
	Serial.println("Entra");
	// Apaga la bomba y el plegador
  digitalWrite(pump,LOW); 	
  digitalWrite(plegador_1,LOW);
  digitalWrite(jet_1,LOW);
  Serial.println("Apaga motores");
}
