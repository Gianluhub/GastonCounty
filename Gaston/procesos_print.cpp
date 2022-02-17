#include <Arduino.h>
#include "procesos.h"


void Llenado_print(int Nivel){

    
    // Comunicacion por serial
    Serial.println("Llenado a nivel " + String(Nivel));
    send_Strmsj("tProceso.txt=\"Llenado de tanque\"");
    Nextion_display(42,0,0,0,0,Nivel,0);

}

void Llamado_op_print(){

    // Comunicacion por serial
    Serial.println("Llamado_op");
    send_Strmsj("tProceso.txt=\"Preparacion de tanque\"");
    Nextion_display(34,0,0,0,0,0,0);
}

void Adicion_rapida_print(int tiempo){

	// Comunicacion por serial
    Serial.println("Adicion_rapida " + String(tiempo) + " min");
    send_Strmsj("tProceso.txt=\"Adicion rapida\"");
    Nextion_display(89,0,0,0,tiempo,0,0);
}

void Adicion_lenta_print(int tiempo, int t_abierto, int t_cerrado){

    Serial.println("Adicion_lenta " + String(tiempo)+" min");
    send_Strmsj("tProceso.txt=\"Adicion lenta\"");
    Nextion_display(88,0,0,0,tiempo,t_abierto,t_cerrado);
}

void Circulacion_print(int interval){

	Serial.println("Circulacion "+ String(interval));
    send_Strmsj("tProceso.txt=\"Circulacion\"");
	Nextion_display(1,0,0,0,interval,0,0);

}

void Lavado_rebose_print(int tiempo){

	 Serial.println("Lavado rebose "+ String(tiempo) + " min");
     send_Strmsj("tProceso.txt=\"Lavado por rebose\"");
	 Nextion_display(50,0,0,0,tiempo,0,0);

}

void Vaciado_print(){

	Serial.println("Vaciado");
    send_Strmsj("tProceso.txt=\"Vaciado\"");
    Nextion_display(62,0,0,0,0,0,0);
}

void Calentamiento_print(int temp, float grad){

	//Serial.println("Calentamiento");
    send_Strmsj("tProceso.txt=\"Calentamiento\"");
    Nextion_display(1,temp,Temp_actual(),grad,0,0,0);
}

void Enfriamiento_print(int temp, float grad){

    // Comunicacion serial
    Serial.println("Enfriamiento");
    send_Strmsj("tProceso.txt=\"Enfriamiento\"");
    Nextion_display(1,temp,Temp_actual(),grad,0,0,0);
}

void Preguntar_print(){

    // Comunicacion por serial
    Serial.println("Preguntar por suavizado");
    Nextion_display(32,0,0,0,0,0,0);
}


void Suavizado_print(){

    // Comunicacion por serial
    Serial.println("Suavizado");
    send_Strmsj("tProceso.txt=\"Suavizado\"");
    Nextion_display(32,0,0,0,0,0,0);
}

void Tomar_muestra_print(){

    // Comunicacion por serial
    Serial.println("Tomar_muestra");
    send_Strmsj("tProceso.txt=\"Toma de muestra\"");
    Nextion_display(32,0,0,0,0,0,0);

}

void Lavado_reductivo_print(){

    // Comunicacion por serial
    Serial.println("Lavado reductivo");
    send_Strmsj("tProceso.txt=\"Lavado reductivo\"");
    Nextion_display(32,0,0,0,0,0,0);

}

void Fin_proceso(){

    // Comunicacion por serial
    Serial.println("Fin del proceso");
    send_Strmsj("tProceso.txt=\"Fin del proceso\"");
    Nextion_display(0,0,0,0,0,0,0);
}
