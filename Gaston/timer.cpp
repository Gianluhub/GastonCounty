/*

TEMPORIZADORES

Si se necesitan mas temporarizadores para procesos en paralelo, copiar y pegar el mismo codigo 
cambiando el nombre a timer2, timer3...

NOTA: LA FUNCION millis() REINICIA SU CONTADOR A 0 UNA VEZ PASADO LOS 50 DIAS.

*/


#include "procesos.h"
#include <Arduino.h>


/*
El parametro de entrada debe estar dado en milisegundos.

La funcion empezara a contar en el momento en que es llamada.
Una vez que pase el tiempo dado por el intervalo, la siguiente vez que se llame a
la funcion comenzara a contar desde cero.
Si se llama la funcion enviandole false como argumento, reinicia el contador para que
empiece a contar la siguiente vez que sea llamada la funcion

*/

// Se esta usando para cada uno de los procesos
int timer1(unsigned long interval){

  unsigned long currentTime = millis();
  static unsigned long previousTime = millis();
  static int start = 0;

  if (interval == false)
  {
    start = 1;
    return false;
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

// Se esta usando para el cerrado de valvulas en Calentamiento
int timer2(unsigned long interval){

  unsigned long currentTime = millis();
  static unsigned long previousTime = millis();
  static int start = 0;

  if (interval == false)
  {
    start = 1;
    return false;
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

// Se esta usando para cerrado de valvulas en Enfriamiento
int timer3(unsigned long interval){

  unsigned long currentTime = millis();
  static unsigned long previousTime = millis();
  static int start = 0;

  if (interval == false)
  {
    start = 1;
    return false;
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

// Usado para actualizar valores de tiempo cada minuto 
int timer4(unsigned long interval){

  unsigned long currentTime = millis();
  static unsigned long previousTime = millis();
  static int start = 0;

  if (interval == false)
  {
    start = 1;
    return false;
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

// Usado para presurizado
int timer5(unsigned long interval){

  unsigned long currentTime = millis();
  static unsigned long previousTime = millis();
  static int start = 0;

  if (interval == false)
  {
    start = 1;
    return false;
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

// Usado para despresurizado
int timer6(unsigned long interval){

  unsigned long currentTime = millis();
  static unsigned long previousTime = millis();
  static int start = 0;

  if (interval == false)
  {
    start = 1;
    return false;
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

// Usado para agregar pequeños delays para enviar datos a la pantalla
int timer7(unsigned long interval){

  unsigned long currentTime = millis();
  static unsigned long previousTime = millis();
  static int start = 0;

  if (interval == false)
  {
    start = 1;
    return false;
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

// Usado para la adicion lenta
int timer8(unsigned long interval){

  unsigned long currentTime = millis();
  static unsigned long previousTime = millis();
  static int start = 0;

  if (interval == false)
  {
    start = 1;
    return false;
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

// Usado para circulacion
int timer9(unsigned long interval){

  unsigned long currentTime = millis();
  static unsigned long previousTime = millis();
  static int start = 0;

  if (interval == false)
  {
    start = 1;
    return false;
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

// Usado para sensor de nivel 1
int timer10(unsigned long interval){

  unsigned long currentTime = millis();
  static unsigned long previousTime = millis();
  static int start = 0;

  if (interval == false)
  {
    start = 1;
    return false;
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

// Usado para sensor de nivel 2
int timer11(unsigned long interval){

  unsigned long currentTime = millis();
  static unsigned long previousTime = millis();
  static int start = 0;

  if (interval == false)
  {
    start = 1;
    return false;
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

// Usado para apagado de bomba
int timer12(unsigned long interval){

  unsigned long currentTime = millis();
  static unsigned long previousTime = millis();
  static int start = 0;

  if (interval == false)
  {
    start = 1;
    return false;
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

// Lectura de sensor para saber si esta encendido
int Sensor_on(unsigned long interval){

  unsigned long currentTime = millis();
  static unsigned long previousTime = millis();
  static int start = 0;
  static bool is_on = false;

  if (interval == false)
  {
    start = 1;
    is_on = false;
    return false;
  }

  if (start == 1)
  { 
    previousTime = millis();
    start = 0;
  }

  if(is_on) return true;

  else if (currentTime - previousTime >= interval)
  {
    is_on = true;
    return true;
  }
  
  return false;

}

// Lectura de sensor para saber si esta apagado
int Sensor_off(unsigned long interval){

  unsigned long currentTime = millis();
  static unsigned long previousTime = millis();
  static int start = 0;
  static bool is_off = false;

  if (interval == false)
  {
    start = 1;
    is_off = false;
    return false;
  }

  if (start == 1)
  { 
    previousTime = millis();
    start = 0;
  }

  if(is_off) return true;

  else if (currentTime - previousTime >= interval)
  {
    is_off = true;
    return true;
  }
  
  return false;

}

// Usado para cerrado de valvul FV209
int timer13(unsigned long interval){

  unsigned long currentTime = millis();
  static unsigned long previousTime = millis();
  static int start = 0;

  if (interval == false)
  {
    start = 1;
    return false;
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
// Funciones auxiliares

//Convierte el tiempo de MINUTOS a milisegundos
unsigned long To_millis(int tiempo){

    unsigned long time = tiempo;
    return time*60*1000;
}


// Reinicia todos los timers
void Reset(){

  timer1(false);
  timer2(false);
  timer3(false);
  Act_tiempo(false); // timer4
  timer5(false);
  timer6(false);
  timer7(false);
  timer8(false);
  timer9(false);
  timer10(false);
  timer11(false);
  timer12(false);
  timer13(false);
  Sensor_on(false);
  Sensor_off(false);
  Reset_PID();
}
