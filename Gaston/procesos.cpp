#include <Arduino.h>
#include "Pines.h"
#include "procesos.h"




/* 
Llenado de tanque
Llena el tanque dependiendo del nivel deseado 
nivel 1 es el nivel bajo y nivel 2 el alto 
*/
void Llenado(int Nivel){
//flags de valvulas (no se estan usando hasta ahora)
    int Bomba  = 0; // Bomba
    int jet   = 0; // jet
    int agua = 0; // Valvula de Agua FV200 
    int reflujo = 0; // Valvula de Reflujo FV204

// Sensores
    int lvl1 = 0; //Sensor de Nivel 1 (Nivel Bajo) LC100
    int lvl2 = 0; // Sensor de Nivel 2 (Nivel Alto) LC101
    
    // Abre valvula para llenar el tanque y asegura que la de reflujo este abierta
    digitalWrite(FV200,HIGH);
    digitalWrite(FV204,HIGH);

    // Cuando llegue al nivel deseado, cierra la valvula y enciende el motor
    switch(Nivel)
    {   
        // Llenado a nivel 1
        case 1:
            if (digitalRead(LC100) >= HIGH)
            {
               digitalWrite(FV200,LOW);
               digitalWrite(pump,LOW); 
            }
        break;

        // Llenado a nivel 2
        case 2:
            if (digitalRead(LC101) >= HIGH)
            {
                digitalWrite(FV201,LOW);
                digitalWrite(pump,LOW);
            }
        break;
    }
}
    

/*
Llamado de operador
Alarma que indica al operador que debe estar presente
No avanza al siguiente paso hasta que atienda el llamado
*/
void Llamado_op(){
    int ok; // flag que debe ser recibido por el operador

    // Enciende la alarma para avisar al operador
    // Y espera a que este responda
    digitalWrite(LLAMADO_OP,HIGH);
     if (Ok == true)
    {
        digitalWrite(LLAMADO_OP,LOW);
    }

}



/*
Adicion rapida
*/

// La variable tiempo estara dada en segundos, hay que convertirla en millisegundos
void Adicion_rapida(int tiempo){

    unsigned long tiempo_ms = To_millis(tiempo); // Pasa el tiempo de segundos a milisegundos 
    delay(tiempo_ms); // Provicional hasta entender bien el flujo del programa 
}


/*
Adicion lenta
*/
void Adicion_lenta(int tiempo, int t_abierto, int t_cerrado ){

    // Pasa el tiempo de segundos a milisegundos
    unsigned long tiempo_ms = To_millis(tiempo);
    unsigned long t_ams = To_millis(t_abierto);
    unsigned long t_cms = To_millis(t_cerrado);
    unsigned long t_inicio = millis();
    unsigned long t_actual = millis();

    // Variables (No se estan usando)
    int call_op = 0;  // llamado op 
    int add_quimico = 0; // adicion de quimico FV206
    int add_flujo = 0;  // adicion de flujo interno FV205
    int add_agua = 0;   // adicion de agua al tanque de quimicos FV207

   
    //MODIFICAR ESTA PARTEEEEEEEEEEEEEEEEEEEEEEEEEEEE
    
    while(t_actual - t_inicio < tiempo_ms)
    {
        /* Abre valvula de adicion cada cierto tiempo */
        digitalWrite(FV206,HIGH);
        delay(t_abierto);
        digitalWrite(FV206,LOW);
        delay(t_cerrado);
        t_actual = millis();
    }


}

// Convierte un tiempo de segundos a milisegundos
unsigned long To_millis(int tiempo){

    return tiempo*1000;

}



void Circulacion (int tiempo){

    unsigned long tiempo_ms = To_millis(tiempo);



}