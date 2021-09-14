#include <Arduino.h>
#include "Pines.h"
#include "setup.h"



void setup() 
{
//  Entradas
//  Falta declarar las entradas analogicas como digitales????
//  Sensores
    pinMode(TC100,INPUT);
    pinMode(LC100,INPUT);
    pinMode(LC101,INPUT);
    pinMode(PCH100,INPUT);
    pinMode(PCL100,INPUT);
    pinMode(LC102,INPUT);


//  Salidas
//  Motores
    pinMode(pump,OUTPUT);
    pinMode(jet_1,OUTPUT);
    pinMode(jet_2,OUTPUT);
    pinMode(jet_3,OUTPUT);
    pinMode(plegador_1,OUTPUT);
    pinMode(plegador_2,OUTPUT);

//  Valvulas
    pinMode(FV200,OUTPUT);
    pinMode(FV201,OUTPUT);
    pinMode(FV202,OUTPUT);
    pinMode(FV203,OUTPUT);
    pinMode(FV204,OUTPUT);
    pinMode(FV205,OUTPUT);
    pinMode(FV206,OUTPUT);
    pinMode(FV207,OUTPUT);
    pinMode(FV208,OUTPUT);
    pinMode(FV209,OUTPUT);
    pinMode(FV210,OUTPUT);
    pinMode(FV211,OUTPUT);
    pinMode(FV212,OUTPUT);
    pinMode(FV213,OUTPUT);
    pinMode(FV214,OUTPUT);

//  Alarmas
    pinMode(LLAMADO_OP,OUTPUT);
    pinMode(TOMA_MUESTRA,OUTPUT);
    pinMode(PRESURIZADO,OUTPUT);
}