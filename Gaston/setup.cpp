
#include <Arduino.h>


// Motores
#define pump 22   // Bomba
#define jet_1 23    // Salida rele 1 del jet
#define jet_2 24    // Salida rele 2 del jet
#define jet_3 25    // Salida 3 del jet
#define plegador_1 26 //Salida rele 1 del pledagor
#define plegador_2 27 //Salida rele 2 del pledagor

// Valvulas
#define FV200 2  // Valvula de agua 
#define FV201 3 // Valvula de agua intercambiador de calor
#define FV202 4 // Valvula de vapor
#define FV203 5 // Valvula de retorno de enfriamiento
#define FV204 6 // Valvula de reflujo
#define FV205 7  // Valvula de flujo interno al tanque de aditivos
#define FV206 8  // Valvula de adicion de quimicos
#define FV207 9  // Valvula de agua al tanque de quimicos
#define FV208 10  // Valvula de trampa
#define FV209 11  // Valvula de purga de intercambiador de calor
#define FV210 12  // Valvula de lavado por revose
#define FV211 13  // Valvula de vaciado
#define FV212 42  // Valvula de presurizado (entrada de aire)
#define FV213 43  // Valvula de despresurizado (salida de aire)
#define FV214 44  // Valvula de regadera

// Sensores
#define TC100 A0  // PT100
#define LC100 A1 // Sensor de Nivel 1 (Nivel Bajo)
#define LC101 A2  // Sensor de Nivel 2 (Nivel Alto)
#define PCH100 A3 // Sensor de presion alto
#define PCL100 A4 // Sensor de presion bajo
#define LC102  A5 // Sensor de vaciado (No implementado)

// Alarmas
#define LLAMADO_OP A6 // Llamado de operador 
#define TOMA_MUESTRA A7 // Tomar muestra
#define PRESURIZADO A8 // Aviso de inicio de presurizacion


// Setup

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
    
}
