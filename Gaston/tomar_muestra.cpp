#include <Arduino.h>
#include <Nextion.h>
#include "Pines.h"
#include "procesos.h"

extern int Nuevo_estado;
extern int Nuevo_estado_ok;
extern int Back;
extern int suav;            
extern int lav_red;

int Tomar_muestra(int estado){

    static int start = true;
    if(start)
    {   
        Tomar_muestra_print();         // Muestra en pantalla
        send_Strmsj("page Preguntar"); // Cambia de pagina para seleccionar el proceso
        start = false;

        // Apaga la bomba y el plegador
        digitalWrite(pump,LOW);
        digitalWrite(plegador_1,LOW);
        //digitalWrite(jet_1,LOW);
    } 
    // Enciende la alarma para avisar al operador
    // Y espera a que este responda
    digitalWrite(LLAMADO_OP,HIGH);
    if ((digitalRead(Op_ok) >= HIGH || Back) && !Nuevo_estado_ok)
    {
        digitalWrite(LLAMADO_OP,LOW);
        send_Strmsj("page Proceso");
        start = true;
        Back = false;

        // Enciende la bomba y el plegador
        digitalWrite(pump,HIGH);
        digitalWrite(plegador_1,HIGH);
        //digitalWrite(jet_1,HIGH);
        delay(200);     // Delay para dar tiempo a que arranquen los motores??
        return estado++;
    }
    else if(Nuevo_estado_ok)
    {   
        digitalWrite(LLAMADO_OP,LOW);
        start = true;
        Nuevo_estado_ok = false;
        estado = Nuevo_estado;

        // Enciende la bomba y el plegador
        digitalWrite(pump,HIGH);
        digitalWrite(plegador_1,HIGH);
        delay(200);
        return estado;
    }
    else if(suav)
    {
        if(Suavizado())
        {
            suav = false;
            Back = true;
        } 
    }
    else if(lav_red)
    {
        if(Lavado_reductivo(80,30))
        {
            lav_red = false;
            Back = true;
        }
    }
    return estado;

}

int Preguntar_Suavizado(){

    static int start = true;
    if(start)
    {   
        Suavizado_print();         // Muestra en pantalla
        send_Strmsj("page Suavizado"); // Cambia de pagina para seleccionar el proceso
        start = false;
    } 
    // Enciende la alarma para avisar al operador
    // Y espera a que este responda
    digitalWrite(LLAMADO_OP,HIGH);
    if ((digitalRead(Op_ok) >= HIGH || Back) && !Nuevo_estado_ok)
    {
        digitalWrite(LLAMADO_OP,LOW);
        start = true;
        Back = false;
        return true;
    }
    else if(Nuevo_estado_ok)
    {   
        digitalWrite(LLAMADO_OP,LOW);
        if(Suavizado())
        {
            start = true;
            Nuevo_estado_ok = false;
            return true;
        }
    }
    return false;
}

int Preguntar_Lavado(){

    static int start = true;
    if(start)
    {   
        Suavizado_print();         // Muestra en pantalla
        send_Strmsj("page Lavado"); // Cambia de pagina para seleccionar el proceso
        start = false;
    } 
    // Enciende la alarma para avisar al operador
    // Y espera a que este responda
    digitalWrite(LLAMADO_OP,HIGH);
    if ((digitalRead(Op_ok) >= HIGH || Back) && !Nuevo_estado_ok)
    {
        digitalWrite(LLAMADO_OP,LOW);
        start = true;
        Back = false;
        return true;
    }
    else if(Nuevo_estado_ok)
    {   
        digitalWrite(LLAMADO_OP,LOW);
        if(Suavizado())
        {
            start = true;
            Nuevo_estado_ok = false;
            return true;
        }
    }
    return false;
}



void Lista_Poliester(int estado, int temperatura, int tiempo){

    Serial.println("Poliester");
    send_msj("nPasoM.val=",estado);
    if(estado<1)
    { 
        estado = 18;
        send_msj("nPasoM.val=",estado);
    }
    else if (estado>18)
    {
        estado= 1;
        send_msj("nPasoM.val=",estado);
    } 

    switch(estado)
    {
        case 1:
            Llenado_print(2);
        break;

        case 2:
            Llamado_op_print();
        break;

        case 3:
            Adicion_rapida_print(5);
        break;

        case 4:
            Circulacion_print(10);
        break;

        case 5:
            Llamado_op_print();
        break;

        case 6:
            Adicion_lenta_print(20,20,2);
        break;

        case 7:
            Circulacion_print(20);
        break;

        case 8:
            Calentamiento_print(temperatura,1.5);
        break;

        case 9:
            Circulacion_print(tiempo);
        break;

        case 10:
            Enfriamiento_print(60,2);
        break;

        case 11:
            Tomar_muestra_print();
        break;

        case 12:
            Lavado_rebose_print(15);
        break;

        case 13:
            Vaciado_print();
        break;

        case 14:
            Llenado_print(2);
        break;

        case 15:
            Lavado_rebose_print(15);
        break;

        case 16:
            Vaciado_print();
        break;

        case 17:
            Tomar_muestra_print();
        break;

        case 18:
            Fin_proceso();
        break;

    }
}


void Lista_Algodon(int estado, int temperatura, int tiempoC, int tiempoF){

    Serial.println("Algodon");
    if(estado<1)
    { 
        estado = 48;
        send_msj("nPasoM.val=",estado);
    }
    else if (estado>48)
    {
        estado= 1;
        send_msj("nPasoM.val=",estado);
    } 

    switch(estado)
    {
        case 1:
            Llenado_print(1);
        break;

        case 2:
            Llamado_op_print();
        break;

        case 3:
            Adicion_rapida_print(30);
        break;

        case 4:
            Circulacion_print(10);
        break;

        case 5:
            Llamado_op_print();
        break;

        case 6:
            Adicion_lenta_print(25,20,2);
        break;

        case 7:
            Calentamiento_print(temperatura,2);
        break;

        case 8:
            Circulacion_print(tiempoC);
        break;

        case 9:
            Llamado_op_print();
        break;

        case 10:
            Adicion_lenta_print(5,10,2);
        break;

        case 11:
            Adicion_lenta_print(25,10,3);
        break;

        case 12:
            Adicion_lenta_print(15,15,3);
        break;

        case 13:
            Circulacion_print(tiempoF);
        break;

        case 14:
            Tomar_muestra_print();
        break;

        case 15:
            Lavado_rebose_print(10);
        break;

        case 16:
            Vaciado_print();
        break;

        case 17:
            Llenado_print(2);
        break;

        case 18:
            Llamado_op_print();
        break;

        case 19:
            Adicion_rapida_print(3);
        break;

        case 20:
            Calentamiento_print(60,2);
        break;

        case 21:
            Circulacion_print(10);
        break;

        case 22:
            Vaciado_print();
        break;

        case 23:
            Llenado_print(2);
        break;

        case 24:
            Llamado_op_print();
        break;

        case 25:
            Adicion_rapida_print(3);
        break;

        case 26:
            Circulacion_print(5);
        break;

        case 27:
            Calentamiento_print(90,2);
        break;

        case 28:
            Circulacion_print(20);
        break;

        case 29:
            Enfriamiento_print(60,2);
        break;

        case 30:
            Vaciado_print();
        break;

        case 31:
            Llenado_print(2);
        break;

        case 32:
            Calentamiento_print(65,2);
        break;

        case 33:
            Circulacion_print(10);
        break;

        case 34:
            Vaciado_print();
        break;

        case 35:
            Llenado_print(2);
        break;

        case 36:
            Tomar_muestra_print();
        break;

        case 37:
            Circulacion_print(10);
        break;

        case 38:
            Vaciado_print();
        break;

        case 39:
            Llenado_print(2);
        break;

        case 40:
            Lavado_rebose_print(15);
        break;

        case 41:
            Vaciado_print();
        break;

        case 42:
            Llenado_print(2);
        break;

        case 43:
            Llamado_op_print();
        break;

        case 44:
            Adicion_rapida_print(3);
        break;

        case 45:
            Circulacion_print(20);
        break;

        case 46:
            Tomar_muestra_print();
        break;

        case 47:
            Vaciado_print();
        break;

        case 48:
            // Fin
            Fin_proceso();
        break;

    }
}


void Lista_preblanqueo_quimico(int estado, int temperatura, int tiempo){

    Serial.println("Preblanqueo Q");
    send_msj("nPasoM.val=",estado);
    if(estado<1)
    { 
        estado = 19;
        send_msj("nPasoM.val=",estado);
    }
    else if (estado>19)
    {
        estado= 1;
        send_msj("nPasoM.val=",estado);
    } 

    switch(estado)
    {
        case 1:
            Llenado_print(2);
        break;

        case 2:
            Llamado_op_print();
        break;

        case 3:
            Adicion_rapida_print(10);
        break;

        case 4:
            Circulacion_print(10);
        break;

        case 5:
            Calentamiento_print(temperatura,2);
        break;

        case 6:
            Circulacion_print(tiempo);
        break;

        case 7:
            Enfriamiento_print(60,2);
        break;

        case 8:
            Tomar_muestra_print();
        break;

        case 9:
            Lavado_rebose_print(10);
        break;

        case 10:
            Vaciado_print();
        break;

        case 11:
            Llenado_print(2);
        break;

        case 12:
            Llamado_op_print();
        break;

        case 13:
            Adicion_rapida_print(3);
        break;

        case 14:
            Circulacion_print(15);
        break;

        case 15:
            Vaciado_print();
        break;

        case 16:
            Llenado_print(2);
        break;

        case 17:
            Lavado_rebose_print(10);
        break;

        case 18:
            Vaciado_print();
        break;

        case 19:
            Fin_proceso();
        break;

    }
}

void Lista_preblanqueo_jabon(int estado, int temperatura, int tiempo){

    Serial.println("Preblanqueo Jabon");
    send_msj("nPasoM.val=",estado);
    if(estado<1)
    { 
        estado = 13;
        send_msj("nPasoM.val=",estado);
    }
    else if (estado>13)
    {
        estado= 1;
        send_msj("nPasoM.val=",estado);
    } 


    switch(estado)
    {
        case 1:
            Llenado_print(2);
        break;

        case 2:
            Llamado_op_print();
        break;

        case 3:
            Adicion_rapida_print(3);
        break;

        case 4:
            Circulacion_print(10);
        break;

        case 5:
            Calentamiento_print(temperatura,2);
        break;

        case 6:
            Circulacion_print(tiempo);
        break;

        case 7:
            Enfriamiento_print(60,2);
        break;

        case 8:
            Lavado_rebose_print(10);
        break;

        case 9:
            Vaciado_print();
        break;

        case 10:
            Llenado_print(2);
        break;

        case 11:
            Lavado_rebose_print(5);
        break;

        case 12:
            Vaciado_print();
        break;

        case 13:
            Fin_proceso();
        break;

    }

}

void Lista_Saponizado(int estado, int temperatura, int tiempo){

    Serial.println("Saponizado");
    send_msj("nPasoM.val=",estado);
    if(estado<1)
    { 
        estado = 23;
        send_msj("nPasoM.val=",estado);
    }
    else if (estado>23)
    {
        estado= 1;
        send_msj("nPasoM.val=",estado);
    } 

    switch(estado)
    {
        case 1:
            Llenado_print(2);
        break;

        case 2:
            Llamado_op_print();
        break;

        case 3:
            Adicion_rapida_print(5);
        break;

        case 4:
            Circulacion_print(10);  
        break;

        case 5:
            Llamado_op_print();
        break;

        case 6:
            Adicion_lenta_print(10,20,2);
        break;

        case 7:
            Calentamiento_print(temperatura,2);
        break;

        case 8:
            Circulacion_print(tiempo);
        break;

        case 9:
            Enfriamiento_print(50,2);
        break;

        case 10:
            Tomar_muestra_print();
        break;

        case 11:
            Lavado_rebose_print(10);
        break;

        case 12:
            Vaciado_print();
        break;

        case 13:
            Llenado_print(2);
        break;

        case 14:
            Llamado_op_print();
        break;

        case 15:
            Adicion_rapida_print(3);    
        break;

        case 16:
            Circulacion_print(20);
        break;

        case 17:
            Vaciado_print();
        break;

        case 18:
            Llenado_print(2);
        break;

        case 19:
            Tomar_muestra_print();
        break;

        case 20:
            Calentamiento_print(60,2);
        break;

        case 21:
            Circulacion_print(10);
        break;

        case 22:
            Vaciado_print();
        break;

        case 23:
            Fin_proceso();
        break;
    }

}

void Lista_Directo(int estado, int temperatura, int tiempo){

    Serial.println("Directo");
    send_msj("nPasoM.val=",estado);
    if(estado<1)
    { 
        estado = 15;
        send_msj("nPasoM.val=",estado);
    }
    else if (estado>15)
    {
        estado= 1;
        send_msj("nPasoM.val=",estado);
    } 

    switch(estado)
    {
        case 1:
            Llenado_print(2);
        break;

        case 2:
            Llamado_op_print();
        break;

        case 3:
            Adicion_rapida_print(5);
        break;

        case 4:
            Circulacion_print(10);
        break;

        case 5:
            Llamado_op_print();
        break;

        case 6:
            Adicion_lenta_print(30,20,2);
        break;

        case 7:
            Circulacion_print(20);
        break;

        case 8:
            Calentamiento_print(temperatura,2);
        break;

        case 9:
            Circulacion_print(tiempo);
        break;

        case 10:
            Enfriamiento_print(60,2);
        break;

        case 11:
            Tomar_muestra_print();
        break;

        case 12:
            Lavado_rebose_print(10);
        break;

        case 13:
            Vaciado_print();
        break;

        case 14:
            Tomar_muestra_print();
        break; 

        case 15:
            Fin_proceso();
        break;
    }
}

void Lista_Lavado_reductivo(int estado, int temperatura, int tiempo){

    Serial.println("Directo");
    send_msj("nPasoM.val=",estado);
    if(estado<1)
    { 
        estado = 13;
        send_msj("nPasoM.val=",estado);
    }
    else if (estado>13)
    {
        estado= 1;
        send_msj("nPasoM.val=",estado);
    }

    switch(estado)
    {
        case 1:
            Llenado_print(2);
        break;

        case 2:
            Adicion_rapida_print(10);
        break;

        case 3: 
            Calentamiento_print(temperatura,2);
        break;

        case 4:
            Circulacion_print(tiempo);
        break;

        case 5:
            Enfriamiento_print(60,2);
        break;

        case 6:
            Tomar_muestra_print();
        break;

        case 7:
            Lavado_rebose_print(15);
        break;

        case 8:
            Vaciado_print();
        break;

        case 9:
            Llenado_print(2);
        break;

        case 10:
            Llamado_op_print();
        break;

        case 11:
            Adicion_rapida_print(3);
        break;

        case 12:
            Circulacion_print(10);
        break;

        case 13:
            Fin_proceso();
        break;
    } 
}


void prueba(int estado, int temperatura, int tiempo){

	Serial.println("Prueba");
    send_msj("nPasoM.val=",estado);
	if(estado<1)
    { 
        estado = 12;
        send_msj("nPasoM.val=",estado);
    }
	else if (estado>12)
    {
        estado= 1;
        send_msj("nPasoM.val=",estado);
    } 

    switch (estado) {

        case 1:
	    	Llenado_print(1);

        case 2:
        	Llenado_print(2);
        break;

        case 3:
   			Llamado_op_print();
        break;

        case 4:
          	Adicion_rapida_print(2);
        break;

        case 5:
          	Adicion_lenta_print(1,2,2);
        break;

        case 6:
          	Circulacion_print(1);
        break;

        case 7:
          	Lavado_rebose_print(1);
        break;

        case 8:
          	Vaciado_print();
        break;

        case 9:
	        Calentamiento_print(130,2);
        break;

        case 10:
          	Enfriamiento_print(60,2);
        break;

        case 11:
            Tomar_muestra_print();
        break;

        case 12:
          Serial.println("Fin de programa");
        break;
    }

}
