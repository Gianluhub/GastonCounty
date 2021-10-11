#include <Arduino.h>
#include "procesos.h"


void Llenado_print(int Nivel){

    // Comunicacion por serial
    Serial.println("Llenado a nivel " + String(Nivel));
    Nextion_display(42,0,0,0,0,0,Nivel);

}


void Llamado_op_print(){

    // Comunicacion por serial
    Serial.println("Llamado_op");
    Nextion_display(34,0,0,0,0,0,0);
}

void Adicion_rapida_print(int tiempo){

	// Comunicacion por serial
    Serial.println("Adicion_rapida " + String(tiempo) + " min");
    Nextion_display(89,0,0,0,tiempo,0,0);
}

void Adicion_lenta_print(int tiempo, int t_abierto, int t_cerrado){

    Serial.println("Adicion_lenta " + String(tiempo)+" min");
    Nextion_display(88,0,0,0,tiempo,t_abierto,t_cerrado);
}

void Circulacion_print(int interval){

	Serial.println("Circulacion "+ String(interval));
	Nextion_display(1,0,0,0,interval,0,0);

}

void Lavado_rebose_print(int tiempo){

	 Serial.println("Lavado rebose "+ String(tiempo) + " min");
	 Nextion_display(50,0,0,0,tiempo,0,0);

}

void Vaciado_print(){

	Serial.println("Vaciado");
    Nextion_display(62,0,0,0,0,0,0);
}

void Calentamiento_print(int temp, float grad){

	Serial.println("Calentamiento");
    Nextion_display(1,temp,Temp_actual(),grad,0,0,0);
}

void Enfriamiento_print(int temp, float grad){

    // Comunicacion serial
    Serial.println("Enfriamiento");
    Nextion_display(1,temp,Temp_actual(),grad,0,0,0);
}

void Preguntar_print(){
    // Comunicacion por serial
    Serial.println("Preguntar por suavizado");
    Nextion_display(32,0,0,0,0,0,0);
}


void Suavizado_print(){

    // Comunicacion por serial
    Serial.println("suavizado");
    Nextion_display(32,0,0,0,0,0,0);
}

void Tomar_muestra_print(){

    // Comunicacion por serial
    Serial.println("Tomar_muestra");
    Nextion_display(32,0,0,0,0,0,0);

}


void Fin_proceso(){

    // Comunicacion por serial
    Serial.println("Fin del proceso");
    Nextion_display(0,0,0,0,0,0,0);
}