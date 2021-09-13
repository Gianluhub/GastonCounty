#include <Arduino.h>
#include "Pines.h"
#include "procesos.h"




/* Llena el tanque dependiendo del nivel deseado, nivel 1 es el nivel bajo y nivel 2 el alto */

void llenado(int Nivel){
//flags de valvulas
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
    
