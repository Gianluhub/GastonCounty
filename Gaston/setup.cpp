#include <Arduino.h>
#include <Nextion.h>
#include "Pines.h"
#include "setup.h"
#include "procesos.h"

// Variables del nextion a inicializar
extern NexButton bNext;
extern NexButton bSelec;
extern NexButton bRight;
extern NexButton bLeft;
extern NexButton bOk;
extern NexButton bSuavSi;
extern NexButton bSuavNo;
extern NexButton bLavRedSi;
extern NexButton bLavRedNo;
extern NexTouch *nex_listen_list[];


void setup() 
{
//  Baud rate
    Serial2.begin(9600);

//  Entradas
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

// Interrupciones y entradas externas
    pinMode(Op_ok, INPUT);
    pinMode(START,INPUT);
    pinMode(STOP,INPUT);
    attachInterrupt(digitalPinToInterrupt(STOP), Interrupt, RISING);

//  Inicio de parametros del Nextion   
    nexInit();
    bNext.attachPop(bNextCallback,&bNext);
    bSelec.attachPop(bSelecCallback,&bSelec);
    bRight.attachPop(bRightCallback,&bRight);
    bOk.attachPop(bCambiarEstadoCallback,&bOk);
    bSuavSi.attachPop(SuavizadoSiCallback,&bSuavSi);
    bSuavNo.attachPop(SuavizadoNoCallback,&bSuavNo);
    bLavRedSi.attachPop(LavadoRedSiCallback,&bLavRedSi);

}
