#include <Arduino.h>
#include <Nextion.h>
#include "Pines.h"
#include "procesos.h"

extern int Nuevo_estado;
extern int Nuevo_estado_ok;
extern int Back;
extern int suav;            
extern int lav_red;
extern int nPaso;
extern int nProc;
//extern int Estado_anterior;

int Tomar_muestra(int estado){

    static int start = true;      // Cambia a la pantalla de toma de muestra, solo se hace una vez 
    //static int motores[3] = {0};  // Controla el estado de los motores
   
    if(start)
    {   
        Tomar_muestra_print();         // Muestra en pantalla
        send_Strmsj("page Preguntar"); // Cambia de pagina para seleccionar el proceso
        start = false;

        // Apaga la bomba y el plegador y torniquete
        digitalWrite(pump,LOW);
        digitalWrite(plegador_1,LOW);
        digitalWrite(jet_1,LOW);
    } 
    // Enciende la alarma para avisar al operador
    // Y espera a que este responda
    digitalWrite(LLAMADO_OP,HIGH);
    if ((digitalRead(Op_ok) >= HIGH || Back) && !Nuevo_estado_ok && !suav && !lav_red)
    {
        digitalWrite(LLAMADO_OP,LOW);
        send_Strmsj("page Proceso");
        start = true;
        Back = false;
        estado++;
        Serial.println("ESTADO");
        Serial.println(estado);
        return estado;
    }
    else if(Nuevo_estado_ok)
    {   
        digitalWrite(LLAMADO_OP,LOW);
        start = true;
        Nuevo_estado_ok = false;
        estado = Nuevo_estado;
        return estado;
    }
    else if(suav)
    {   
        //Estado_anterior = estado;
        start = true;     
        suav = false;
        return 100;   
    }
    else if(lav_red)
    {   
        //Estado_anterior = estado;
        start = true;
        lav_red = false;
        return 101;
        
    }
    return estado;

}



void Lista_Poliester(int estado, int temperatura, int tiempo){

    Serial.println("Poliester");
    send_msj("nPasoM.val=",estado);
    send_msj("nProc.val=",4);
    if(estado<1)
    { 
        estado = 18;
        nPaso = 18;
        send_msj("nPasoM.val=",estado);
    }
    else if (estado>18)
    {
        estado= 1;
        nPaso = 1;
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
    send_msj("nProc.val=",5);
    if(estado<1)
    { 
        estado = 48;
        nPaso = 48;
        send_msj("nPasoM.val=",estado);
    }
    else if (estado>48)
    {
        estado= 1;
        nPaso = 1;
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
    send_msj("nProc.val=",1);
    if(estado<1)
    { 
        estado = 19;
        nPaso = 19;
        send_msj("nPasoM.val=",estado);
    }
    else if (estado>19)
    {
        estado= 1;
        nPaso = 1;
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
    send_msj("nProc.val=",2);
    if(estado<1)
    { 
        estado = 14;
        nPaso = 14;
        send_msj("nPasoM.val=",estado);
    }
    else if (estado>14)
    {
        estado = 1;
        nPaso  = 1;
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
            Lavado_rebose_print(5);
        break;

        case 13:
            Vaciado_print();
        break;

        case 14:
            Fin_proceso();
        break;

    }

}

void Lista_Saponizado(int estado, int temperatura, int tiempo){

    Serial.println("Saponizado");
    send_msj("nPasoM.val=",estado);
    send_msj("nProc.val=",3);
    if(estado<1)
    { 
        estado = 23;
        nPaso = 23;
        send_msj("nPasoM.val=",estado);
    }
    else if (estado>23)
    {
        estado = 1;
        nPaso = 1;
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
    send_msj("nProc.val=",6);
    if(estado<1)
    { 
        estado = 15;
        nPaso = 15;
        send_msj("nPasoM.val=",estado);
    }
    else if (estado>15)
    {
        estado= 1;
        nPaso = 1;
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
    send_msj("nProc.val=",8);
    if(estado<1)
    { 
        estado = 14;
        nPaso = 14;
        send_msj("nPasoM.val=",estado);
    }
    else if (estado>14)
    {
        estado = 1;
        nPaso = 1;
        send_msj("nPasoM.val=",estado);
    }

    switch(estado)
    {
        
        case 1:
            Vaciado_print();
        break;

        case 2:
            Llenado_print(2);
        break;

        case 3:
            Adicion_rapida_print(10);
        break;

        case 4: 
            Calentamiento_print(temperatura,2);
        break;

        case 5:
            Circulacion_print(tiempo);
        break;

        case 6:
            Enfriamiento_print(60,2);
        break;

        case 7:
            Tomar_muestra_print();
        break;

        case 8:
            Lavado_rebose_print(15);
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
            Adicion_rapida_print(3);
        break;

        case 13:
            Circulacion_print(10);
        break;

        case 14:
            Fin_proceso();
        break;
    } 
}


void Lista_Suavizado(int estado){

    Serial.println("Suavizado");
    send_msj("nPasoM.val=",estado);
    send_msj("nProc.val=",7);
    if(estado<1)
    { 
        estado = 6;
        nPaso = 6;
        send_msj("nPasoM.val=",estado);
    }
    else if (estado>6)
    {
        estado = 1;
        nPaso = 1;
        send_msj("nPasoM.val=",estado);
    }

    switch(estado)
    {
        
        case 1:
            Vaciado_print();
        break;

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
            Circulacion_print(20);
        break;

        case 6:
            Fin_proceso();
        break;
    }
}


void Lista_Lavado_Maquina(int estado){

    Serial.println("Lavado de maquina");
    send_msj("nPasoM.val=",estado);
    send_msj("nProc.val=",10);
    if(estado<1)
    { 
        estado = 13;
        nPaso = 13;
        send_msj("nPasoM.val=",estado);
    }
    else if (estado>13)
    {
        estado= 1;
        nPaso = 1;
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
            Calentamiento_print(130,0);
        break;

        case 5:
            Circulacion_print(60);
        break;

        case 6:
            Enfriamiento_print(60,0);
        break;

        case 7:
            Vaciado_print();
        break;

        case 8:
            Llenado_print(2);
        break;

        case 9:
            Llamado_op_print();
        break;

        case 10:
            Adicion_rapida_print(5);
        break;

        case 11:
            Circulacion_print(15);
        break;

        case 12:
            Vaciado_print();
        break;

        case 13:
            Fin_proceso();
        break;
    }

}


void prueba(int estado, int temperatura, int tiempo){

    Serial.println("Prueba");
    send_msj("nPasoM.val=",estado);
    send_msj("nProc.val=",9);
    if(estado<1)
    { 
        estado = 9;
        nPaso = 9;
        send_msj("nPasoM.val=",estado);
    }
    else if (estado>9)
    {
        estado= 1;
        nPaso = 1;
        send_msj("nPasoM.val=",estado);
    } 

    switch (estado) {

        case 1:
            Llenado_print(2);
        break;

        case 2:
            Llamado_op_print();
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
            Lavado_rebose_print(10);
        break;

        case 8:
            Vaciado_print();
        break;

        case 9:
            Fin_proceso();
        break;
    }

}

// void prueba(int estado, int temperatura, int tiempo){

//     Serial.println("Prueba");
//     send_msj("nPasoM.val=",estado);
//     send_msj("nProc.val=",9);
//     if(estado<1)
//     { 
//         estado = 12;
//         nPaso = 12;
//         send_msj("nPasoM.val=",estado);
//     }
//     else if (estado>12)
//     {
//         estado= 1;
//         nPaso = 1;
//         send_msj("nPasoM.val=",estado);
//     } 

//     switch (estado) {

//         case 1:
//             Llenado_print(1);
//         break;
        
//         case 2:
//             Llenado_print(2);
//         break;

//         case 3:
//             Llamado_op_print();
//         break;

//         case 4:
//             Adicion_rapida_print(2);
//         break;

//         case 5:
//             Adicion_lenta_print(1,2,2);
//         break;

//         case 6:
//             Circulacion_print(1);
//         break;

//         case 7:
//             Lavado_rebose_print(1);
//         break;

//         case 8:
//             Vaciado_print();
//         break;

//         case 9:
//             Calentamiento_print(130,2);
//         break;

//         case 10:
//             Enfriamiento_print(60,2);
//         break;

//         case 11:
//             Tomar_muestra_print();
//         break;

//         case 12:
//             Fin_proceso();
//         break;
//     }

// }