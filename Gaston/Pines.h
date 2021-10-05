#ifndef pines_h
#define pines_h
#include <SPI.h>
#include <Controllino.h>
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
#define TC100 CONTROLLINO_A0  // Sensor de temperatura
#define LC100 CONTROLLINO_A1 // Sensor de Nivel 1 (Nivel Bajo)
#define LC101 CONTROLLINO_A2  // Sensor de Nivel 2 (Nivel Alto)
#define PCH100 CONTROLLINO_A3 // Sensor de presion alto
#define PCL100 CONTROLLINO_A4 // Sensor de presion bajo
#define LC102  CONTROLLINO_A5 // Sensor de vaciado (No implementado)


// Alarmas
#define LLAMADO_OP 28   // Llamado de operador 
#define TOMA_MUESTRA 28 // Tomar muestra
#define PRESURIZADO 30  // Aviso de inicio de presurizacion

// Interrupciones y entradas externas
#define Op_ok CONTROLLINO_A6 // Interruptor de llamado de operador
#define Start CONTROLLINO_A7 // Interruptor de inicio de proceso
#define Stop  38			 // Interrupcion para stop del proceso

#endif
