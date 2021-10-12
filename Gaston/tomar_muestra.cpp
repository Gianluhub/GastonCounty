#include <Arduino.h>
#include <Nextion.h>
#include "Pines.h"
#include "procesos.h"

extern int Nuevo_estado;
extern int Nuevo_estado_ok;



int Tomar_muestra(int estado){

    static int start = true;
    if(start)
    {   
        Tomar_muestra_print();
        // Cambia de pagina para seleccionar el proceso
        send_Strmsj("page Preguntar");
    } 
    // Enciende la alarma para avisar al operador
    // Y espera a que este responda
    digitalWrite(LLAMADO_OP,HIGH);
    if (digitalRead(Op_ok) >= HIGH)
    {
        digitalWrite(LLAMADO_OP,LOW);
        send_Strmsj("page proceso");
        start = true;
        return estado++;
    }
    else if(Nuevo_estado_ok)
    {   
        digitalWrite(LLAMADO_OP,LOW);
        start = true;
        Nuevo_estado_ok = false;
        estado = Nuevo_estado;
    }
    return estado;

}




void Lista_Poliester(int estado, int temperatura, int tiempo){

    send_msj("nPasoM.val=",estado);
    if(estado<1) estado = 18;
    else if (estado>18) estado= 1;

    switch(estado)
    {
        case 1:
            Llenado_print(2);
        break;

        case 2:
            Llamado_op_print();
        break;

        case 3:
            Adicion_rapida_print(4);
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
            Lavado_rebose_print(15);
        break;

        case 12:
            Vaciado_print();
        break;

        case 13:
            Llenado_print(2);
        break;

        case 14:
            Lavado_rebose_print(15);
        break;

        case 15:
            Vaciado_print();
        break;

        case 16:
            Preguntar_print();
        break;

        case 17:
            Suavizado_print();
        break;

        case 18:
            Fin_proceso();
        break;

    }
}


void Lista_Algodon(int estado, int temperatura, int tiempo){

    send_msj("nPasoM.val=",estado);
    if(estado<1) estado = 48;
    else if (estado>48) estado= 1;

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
            Circulacion_print(tiempo);
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
            Circulacion_print(60);
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
            Llenado_print(1);
        break;

        case 18:
            Circulacion_print(10);
        break;

        case 19:
            Vaciado_print();
        break;

        case 20:
            Llenado_print(1);
        break;

        case 21:
            Llamado_op_print();
        break;

        case 22:
            Adicion_rapida_print(3);
        break;

        case 23:
            Circulacion_print(5);
        break;

        case 24:
            Calentamiento_print(90,2);
        break;

        case 25:
            Circulacion_print(20);
        break;

        case 26:
            Enfriamiento_print(60,2);
        break;

        case 27:
            Vaciado_print();
        break;

        case 28:
            Llenado_print(1);
        break;

        case 29:
            Calentamiento_print(65,2);
        break;

        case 30:
            Circulacion_print(10);
        break;

        case 31:
            Vaciado_print();
        break;

        case 32:
            Llenado_print(1);
        break;

        case 33:
            Circulacion_print(10);
        break;

        case 34:
            Vaciado_print();
        break;

        case 35:
            Llenado_print(1);
        break;

        case 36:
            Lavado_rebose_print(15);
        break;

        case 37:
            Vaciado_print();
        break;

        case 38:
            Llenado_print(1);
        break;

        case 39:
            Llamado_op_print();
        break;

        case 40:
            Adicion_rapida_print(5);
        break;

        case 41:
            Llamado_op_print();
        break;

        case 42:
            Adicion_rapida_print(5);
        break;

        case 43:
            Circulacion_print(20);
        break;

        case 44:
            Preguntar_print();
        break;

        case 45:
            Suavizado_print();
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


void Lista_preblanqueo_quimico(int estado){

    send_msj("nPasoM.val=",estado);
    if(estado<1) estado = 18;
    else if (estado>18) estado= 1;

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
            Calentamiento_print(98,2);
        break;

        case 6:
            Circulacion_print(30);
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
            Llamado_op_print();
        break;

        case 12:
            Adicion_rapida_print(15);
        break;

        case 13:
            Circulacion_print(10);
        break;

        case 14:
            Vaciado_print();
        break;

        case 15:
            Llenado_print(2);
        break;

        case 16:
            Lavado_rebose_print(10);
        break;

        case 17:
            Vaciado_print();
        break;

        case 18:
            Fin_proceso();
        break;

    }
}

void Lista_preblanqueo_jabon(int estado){

    send_msj("nPasoM.val=",estado);
    if(estado<1) estado = 13;
    else if (estado>13) estado= 1;

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
            Calentamiento_print(98,2);
        break;

        case 6:
            Circulacion_print(30);
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

void Lista_Saponizado(int estado){

    send_msj("nPasoM.val=",estado);
    if(estado<1) estado = 22;
    else if (estado>22) estado= 1;

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
            Calentamiento_print(105,1.5);
        break;

        case 8:
            Circulacion_print(40);
        break;

        case 9:
            Enfriamiento_print(50,1.5);
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
            Calentamiento_print(60,2);
        break;

        case 20:
            Circulacion_print(10);
        break;

        case 21:
            Vaciado_print();
        break;

        case 22:
            Fin_proceso();
        break;
    }

}


void prueba(int estado){

	send_msj("nPasoM.val=",estado);
	if(estado<1) estado = 11;
	else if (estado>11) estado= 1;

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
