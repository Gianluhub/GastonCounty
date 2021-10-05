/*

TEMPORIZADORES

Si se necesitan mas temporarizadores para procesos en paralelo, copiar y pergar el mismo codigo 
cambiando el nombre a timer2, timer3...

NOTA: LA FUNCION millis() REINICIA SU CONTADOR A 0 UNA VEZ PASADO LOS 50 DIAS. FALTA IMPLEMENTAR LA SOLUCION A ESTE PROBLEMA.

*/


#include "procesos.h"
#include <Arduino.h>


/*
El parametro de entrada debe estar dado en milisegundos.

La funcion empezara a contar en el momento en que es llamada.
Una vez que pase el tiempo dado por el intervalo, la siguiente vez que se llame a
la funcion comenzara a contar desde cero.

*/

// Se esta usando para cada uno de los procesos
int timer1(unsigned long interval){

  unsigned long currentTime = millis();
  static unsigned long previousTime = millis();
  static int start = 0;

  if (start == 1)
  { 
    previousTime = millis();
    start = 0;
  }

  if (currentTime - previousTime >= interval)
  {
    start = 1;
    return true;
  }
  else return false;

}

// Se esta usando para el cerrado de valvulas en Calentamiento
int timer2(unsigned long interval){

  unsigned long currentTime = millis();
  static unsigned long previousTime = millis();
  static int start = 0;

  if (start == 1)
  { 
    previousTime = millis();
    start = 0;
  }

  if (currentTime - previousTime >= interval)
  {
    start = 1;
    return true;
  }
  else return false;

}

// Se esta usando para cerrado de valvulas en Enfriamiento
int timer3(unsigned long interval){

  unsigned long currentTime = millis();
  static unsigned long previousTime = millis();
  static int start = 0;

  if (start == 1)
  { 
    previousTime = millis();
    start = 0;
  }

  if (currentTime - previousTime >= interval)
  {
    start = 1;
    return true;
  }
  else return false;

}

// Usado para actualizar valores de tiempo 
int timer4(unsigned long interval, int reset = false){

  unsigned long currentTime = millis();
  static unsigned long previousTime = millis();
  static int start = 0;

  if (reset)
  {
    start = 1;
    return true;
  }

  if (start == 1)
  { 
    previousTime = millis();
    start = 0;
  }

  if (currentTime - previousTime >= interval)
  {
    start = 1;
    return true;
  }
  else return false;

}


// Usado para actualizar valores de temperatura
int timer5(unsigned long interval, int reset = false){

  unsigned long currentTime = millis();
  static unsigned long previousTime = millis();
  static int start = 0;

  if (reset)
  {
    start = 1;
    return true;
  }

  if (start == 1)
  { 
    previousTime = millis();
    start = 0;
  }

  if (currentTime - previousTime >= interval)
  {
    start = 1;
    return true;
  }
  else return false;

}

// Funcion auxiliar
//Convierte el tiempo de MINUTOS a milisegundos
unsigned long To_millis(int tiempo){

    return tiempo*60*1000;

}
