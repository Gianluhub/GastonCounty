#include <Arduino.h>
#include "Pines.h"
#include "procesos.h"




/* 
Llenado de tanque
Llena el tanque dependiendo del nivel deseado 
nivel 1 es el nivel bajo y nivel 2 el alto 
*/
// FALTA AVERIGUAR SOBRE EL ENCENDIDO DEL PLEGADOR
void Llenado(int Nivel){
//flags de valvulas (no se estan usando hasta ahora)
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

    // Cuando llegue al nivel deseado, cierra la valvula, enciendo el motor y el plegador
    switch(Nivel)
    {   
        // Llenado a nivel 1
        case 1:
            if (digitalRead(LC100) >= HIGH)
            {
               digitalWrite(FV200,LOW);
               digitalWrite(pump,HIGH); 
               digitalWrite(plegador_1,HIGH);
               digitalWrite(plegador_2,HIGH);
            }
        break;

        // Llenado a nivel 2
        case 2:
            if (digitalRead(LC101) >= HIGH)
            {
                digitalWrite(FV201,LOW);
                digitalWrite(pump,HIGH);
                digitalWrite(plegador_1,HIGH);
                digitalWrite(plegador_2,HIGH);
            }
        break;
    }
}
    

/*
Llamado de operador
Alarma que indica al operador que debe estar presente
No avanza al siguiente paso hasta que atienda el llamado
*/
void Llamado_op(){
    int ok; // flag que debe ser recibido por el operador

    // Enciende la alarma para avisar al operador
    // Y espera a que este responda
    digitalWrite(LLAMADO_OP,HIGH);
     if (ok == true)
    {
        digitalWrite(LLAMADO_OP,LOW);
    }

}



/*
Adicion rapida
*/

// La variable tiempo estara dada en segundos, hay que convertirla en millisegundos
void Adicion_rapida(int tiempo){

    unsigned long tiempo_ms = To_millis(tiempo); // Pasa el tiempo de segundos a milisegundos 
    delay(tiempo_ms); // Provicional hasta entender bien el flujo del programa 
}


/*
Adicion lenta
Los parametros de entradas deben estar dados en segundos
*/
void Adicion_lenta(int tiempo, int t_abierto, int t_cerrado ){

    // Pasa el tiempo de segundos a milisegundos
    unsigned long tiempo_ms = To_millis(tiempo);
    unsigned long t_ams = To_millis(t_abierto);
    unsigned long t_cms = To_millis(t_cerrado);
    unsigned long t_inicio = millis();
    unsigned long t_actual = millis();

    // Variables (No se estan usando)
    int call_op = 0;  // llamado op 
    int add_quimico = 0; // adicion de quimico FV206
    int add_flujo = 0;  // adicion de flujo interno FV205
    int add_agua = 0;   // adicion de agua al tanque de quimicos FV207

    // Mientras que no haya pasado el tiempo se seguiran abriendo la valvulas     
    while (!timer1(tiempo_ms))
    {
        /* Abre valvula de adicion cada cierto tiempo */
        digitalWrite(FV206,HIGH);
        delay(t_abierto);
        digitalWrite(FV206,LOW);
        delay(t_cerrado);
    }
}


/*
 Circulacion
 Es la misma funcion timer, debe recibir el tiempo en milisegundos.
*/

int Circulacion(unsigned long interval){

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


/*
 Lavado por rebose
 El parametro de tiempo esta en segundos.
 Mientras que corre el temporizador se estara realizando lecturas de 
 los sensores de nivel, cuando esta a nivel 2, abre la valvula de vaciado 
 por rebose FV210. Cuando el nivel 1 esta en LOW abre la valvula de agua 
 y cierra la de vaciado.
*/


void Lavado_rebose(int tiempo){

    // Variables usadas
    int rebose; // Valvula FV210 salida por rebose
    int agua;   // Valvula de agua FV200
    int lvl1;   // Sensor de nivel 1 LC100
    int lvl2;   // Sensor de nivel 2 LC101
    unsigned long tiempo_ms = To_millis(tiempo);
    // Se abre valvula de vaciado FV210
    digitalWrite(FV210,HIGH);

    /*
     Mientras que el temporizador esta contando, se va 
     realizando el lavado por rebose
    */
    while (!timer1(tiempo_ms))
    {   

        // Si el sensor de nivel 2 esta en HIGH
        // Abre la valvula de vaciado y cierra la de llenado
        if (digitalRead(LC101) >= HIGH)
        {
            digitalWrite(FV210,HIGH);
            digitalWrite(FV200,LOW);
        }
        // Si el sensor de nivel 1 esta en LOW
        // Abre la valvula de llenado y cierra la de vaciado
        if (digitalRead(LC100) <= LOW)
        {
            digitalWrite(FV210,LOW);
            digitalWrite(FV200,HIGH);
        }
    }

    // Al finalizar el tiempo se asegura que las valvulas esten cerradas
    digitalWrite(FV210,LOW);
    digitalWrite(FV200,LOW);
}



/*
 Vaciado 
 Se hace por tiempo, hay que estimar el tiempo en que tarda en vaciar el tanque
 no requiere ningun parametro de entrada.
*/

// NO ESTA CLARO SI EL PLEGADOR SE INICIA CON LA FUNCION digitalwrite
void vaciado(){

    const unsigned long tiempo_vaciado = 1000;  // Falta poner el tiempo
    // Se abre la valvula de vaciado y se apaga la bomba y el plegador
    digitalWrite(pump,LOW);
    digitalWrite(plegador_1,LOW);
    digitalWrite(plegador_2,LOW);
    digitalWrite(FV211,HIGH);
    // Espera el tiempo necesario donde se sabe que el tanque esta vacio
    // y luego cierra la valvula
    delay(tiempo_vaciado);
    digitalWrite(FV211,LOW);

}




/*
    Calentamiento de tanque
    Recibe como entrada temperatura, gradiente y tiempo (aun no implementado)
    Si el gradiente es 0 significa que se subira la temperatura de forma directa
    es decir con las valvulas siempre abiertas.
*/

// NO ESTA CLARO AUN SI LAS VALVULAS DE TRAMPA Y PURGA DEBEN ESTAR CERRADAS DURANTE PRESURIZACION

void Calentamiento(int temp, int grad, int tiempo){


    // Valvulas usadas
    int vapor; // Valvula de vapor FV202
    int trampa; // Valvula de trampa FV208
    int purga; // Valvula de purga FV209


    // Sensores
    int sensor_tem; // Sensor PT100 - TC100

    // Variables de utilidad
    unsigned long t_abierto;
    unsigned long t_cerrado;

    // FALTA REALIZAR UNA RELACION ENTRE EL GRADIENTE Y LOS TIEMPOS
    //  Y CUANDO ENTRA EN PRESURIZACION

    while (digitalRead(TC100) <= temp)
    {   
        // Si no se requiere gradiente deja las valvulas abiertas siempre
        // No esta claro si en este caso tambien hay que dejar SIEMPRE abiertas
        // las valvulas de purga y de la trampa
        if (grad == 0)
        {
            digitalWrite(FV202,HIGH);
            digitalWrite(FV208,HIGH);
            digitalWrite(FV209,HIGH);
        }
        else
        {
            // AQUI FALTA IMPLEMENTAR LOS TIEMPOS CON EL GRADIENTE
            // Como esta dentro de un while no se esta considerando aun  la presurizacion
            digitalWrite(FV202,HIGH);
            digitalWrite(FV208,HIGH);
            digitalWrite(FV209,HIGH);
            delay(t_abierto);
            digitalWrite(FV202,LOW);
            digitalWrite(FV208,LOW);
            digitalWrite(FV209,LOW);
            delay(t_cerrado);

        }
    }

    // Asegurar que las valvulas estan cerradas
    digitalWrite(FV202,LOW);
    digitalWrite(FV208,LOW);
    digitalWrite(FV209,LOW);
}






/*
    Enfriamiento del tanque
    Recibe como entrada temperatura, gradiente y tiempo (aun no implementado)
    Si el gradiente es 0 significa que se subira la temperatura de forma directa
    es decir con las valvulas siempre abiertas.
*/

// NO ESTA CLARO AUN SI LAS VALVULAS DE TRAMPA Y PURGA DEBEN ESTAR CERRADAS DURANTE PRESURIZACION

void Enfriamiento(int temp, int grad, int tiempo){


    // Valvulas usadas
    int add_agua; // Valvula de agua FV201
    int retorno; // Valvula de retorno de enfriamiento FV203


    // Sensores
    int sensor_tem; // Sensor PT100 - TC100

    // Variables de utilidad
    unsigned long t_abierto;
    unsigned long t_cerrado;

    // FALTA REALIZAR UNA RELACION ENTRE EL GRADIENTE Y LOS TIEMPOS
    //  Y CUANDO ENTRA EN PRESURIZACION

    while (digitalRead(TC100) <= temp)
    {   
        // Si no se requiere gradiente deja las valvulas abiertas siempre
    
        if (grad == 0)
        {
            digitalWrite(FV201,HIGH);
            digitalWrite(FV203,HIGH);
        
        }
        else
        {
            // AQUI FALTA IMPLEMENTAR LOS TIEMPOS CON EL GRADIENTE
            // Como esta dentro de un while no se esta considerando aun  la presurizacion
            digitalWrite(FV201,HIGH);
            digitalWrite(FV203,HIGH);
            delay(t_abierto);
            digitalWrite(FV201,LOW);
            digitalWrite(FV203,LOW);
            delay(t_cerrado);

        }
    }

    // Asegurar que las valvulas estan cerradas
    digitalWrite(FV201,LOW);
    digitalWrite(FV203,LOW);
}






/*
 Presurizacion
*/

void Presurizado(){

    // Valvulas
    int aire; // valvula de presurizacion FV212
    int vapor; // Valvula de vapor FV202 
    int master; // Cerrar todas las valvulas sobretodo la de reflujo FV204

    // Sensores
    int temp; // Sensor de temperatura TC100
    int Phi; // Sensor de presion alta PCH100
    


}


void Despresurizado(){

    int salida_aire; // Valvula de despresurizado FV213
    int aire; // valvula de presurizado FV212 (debe cerrarse)
    int reflujo; //valvula de reflujo FV204

    //Sensores
    int Plo; // Sensore de presion baja PCL100
    int temp; // Sensore de temperatura TC100


    
}