#include <SPI.h>
#include <Controllino.h>
#include <Nextion.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "Pines.h"
#include "setup.h"
#include "procesos.h"


/*
  Variables utilizadas para almacenar data proveniente del Nextion
  * buffer: Recibe la data proveniente del Nextion.
  * trama: Contiene la data desentramada utilizada para iniciar los procesos.
  		   Cada indice del array contiene el estado del proceso que se desea ejecutar. 
  * temperatura : Almacena la temperatura que alcanzara el tanque al iniciar el proceso de teñido.
  				  Para el poliester la temepratura se almacena en el indice 0 y para el algodon en el 1.
*/ 
char buffer[50] = {0};		
char trama[20] = {0};		
int temperatura[2] = {0};	
int tiempo[2] = {0};



// Declaracion de objeto que representa los botones del Nextion
NexButton bNext=NexButton(7,1,"bNext");



// Declaracion de lista de eventos de los botones del Nextion
NexTouch *nex_listen_list[] = {
  &bNext,
  NULL
};


/*
   Funciones que seran llamadas al producirse un evento
*/

void bNextCallback(void*ptr){

	memset(buffer, 0, sizeof(buffer));  	// Limpia el buffer para recibir la data
	bNext.getText(buffer, sizeof(buffer));  // Recibe los datos y se almacenan en el buffer					
	desentramado(trama,temperatura,tiempo);		// Separa los datos para inciar el proceso

	// Imprime por serial los valores para ser monitoreados
	Serial.println(buffer);	
	Serial.println(trama);
	Serial.println(temperatura[0]);   // Temperatura para el poliester
  Serial.println(tiempo[0]);        // Tiempo para el poliester
	Serial.println(temperatura[1]);   // Temperatura para el algodon
  Serial.println(tiempo[1]);        // Tiempo para el algodon	
}


/*
	Lee los datos del buffer y toma los datos de interes
	la trama recibida viene dada de en la forma: +A+D-130-60-+X
	Donde las letras representan un proceso como preblanqueo o poliester, los numeros representan
	unidades de temperatura y tiempo en ese mismo orden y la X se usa para indicar fin de la trama
*/
void desentramado(char trama[],int temperatura[], int tiempo[]){
  int i = 0;  // Indice del buffer
  int j = 0;  // Indice de trama

  // Limpia los arrays
  memset(trama, 0, sizeof(trama)); 
  memset(temperatura, 0, sizeof(temperatura));
  memset(tiempo, 0, sizeof(tiempo)); 
 
  
  // Extrae los datos hasta que se lea el caracter de fin 'X'
  do
  {
    // Revisa si hay un dato nuevo que tomar
    if(buffer[i] == '+')
    {
      trama[j] = buffer[i+1]; // Toma el dato siguiente
      i+=2;                   // Incrementa en dos para saltar al siguiente dato a extraer
      j++;                    // Incrementa la posicion del array datos
    }

    // Extrae la temperatura de la trama
    i = Tomar_Dato(i,'-',buffer,temperatura);
    i = Tomar_Dato(i,'#',buffer,tiempo);
    
  }while(buffer[i]!='X');
  trama[j] = 'X';
}


/*
  Extrae los datos de interes al detectar un caracter de inicio y guarda
  los datos en formato entero.
*/
int Tomar_Dato(int i,char start, char buffer[], int save[2]){
  int k = 0;  // Indice de aux
  char aux[4] = {0};  // Array auxiliar para las temperaturas
  char aux2 = 0;

  // Verifica si es necesario extraer un valor de temperatura
  if (buffer[i] == start)
  {
    aux2 = buffer[i+1];     // Guarda el caracter para saber si es de poliester o algodon
    i+=2;                   // Incrementa al siguiente dato a extraer

    // Extrae los valores de temperatura hasta que se lea el caracter de fin '-'
    do
    {
      aux[k] = buffer[i];
      k++;
      i++;
    }while(buffer[i]!=start);

    i++;   // Aumenta el indice de trama para la siguiente iteracion
    
    // Si la temperatura o el tiempo pertenece a la del poliester guarda ese valor en la posicion 0 del array
    if(aux2 == 'p') save[0] = String(aux).toInt();  // Convierte los caracteres a un entero y se guarda    
  
    // Si la temperatura o el tiempo pertenece a la del algodon guarda ese valor en la posicion 1 del array
    else if(aux2 == 'a') save[1] = String(aux).toInt();  // Convierte los caracteres a un entero y se guarda    
  }
  return i;

}


// Envia datos al HMI para ser mostrados en pantalla
void send_msj(char msj[],int dato){

  Serial2.print(msj);  // Se envia un string que hace referencia a la variable del HMI que se desea modificar ejemplo "nTemp.val="
  Serial2.print(dato);  // Este es el dato que se guardara en la variable msj
  Serial2.write(0xff);  // Indica fin de la trama
  Serial2.write(0xff);
  Serial2.write(0xff);
}

void Nextion_display(int Cod, int Temp, int TempA, int Grad, int Tiempo, int Aper, int Cierre ){

  send_msj("nCod.val=",Cod);
  send_msj("nTemp.val=",Temp);
  send_msj("nTempA.val=",TempA);
  send_msj("nGrad.val=",Grad);
  send_msj("nTiempo.val=",Tiempo);
  //send_msj("nPaso.val=",Paso);
  send_msj("nAper.val=",Aper);
  send_msj("nCierre.val=",Cierre);
}



// Maquinas de estados





void loop(){

	static int estado = 0;   // Esta variable recorrera los estados del switch segun lo contenido en el array trama
	nexLoop(nex_listen_list); // Verifica si se reciben datos del HMI

    //if(digitalRead(Start)>=HIGH) estado+=1;
    switch (estado) {
        case 0:
          Serial.println("case 0");
          if(digitalRead(Start)>=HIGH) estado++;  
        break;

        case 1:
          if(Llenado(1)) estado++;
        break;

        case 2:
          if(Llenado(2)) estado++;
        break;

        case 3:
          if(Llamado_op()) estado++;
        break;

        case 4:
          if(Adicion_rapida(1)) estado++;
        break;

        case 5:
          if(Adicion_lenta(1,2000,2000)) estado++;
        break;

        case 6:
          if(Circulacion(1)) estado++;
        break;

        case 7:
          if(Lavado_rebose(2)) estado++;
        break;

        case 8:
          if(Vaciado()) estado++;
        break;

        case 9:
          Calentamiento(130,2);
          Presurizado();
          Serial.println(Temp_actual());
          if(Temp_actual() >= 130) estado++;
        break;

        case 10:
          Enfriamiento(60,2);
          Despresurizado();
          Serial.println(Temp_actual());
          if(Temp_actual() <= 60) estado++;
        break;

        case 11:
          Serial.println("Fin de programa");
          estado = 0;
        break;


       
    }



//  Serial.println("Temperatura ");
//  Serial.println(Temp_actual());
//  Serial.println("Lectura ");
//  Serial.println(analogRead(TC100));
 // Temp_actual();
  //delay(2000);
  
//  switch (trama[estado]) {
//
//      // Preblanqueo quimico
//      case 'A':
//        
//        Serial.println("Preblanqueo quimico\n");
//        if(Preblanqueo_quimico()) estado++;
//      break;
//
//      case 'B':
//
//        Serial.println("Preblanqueo con jabon\n");
//        
//        estado++;
//      break;
//
//      case 'C':
//        
//      break;
//
//
//      case 'D':
//        Serial.println("Poliester a ");
//        if(Poliester(temperatura[0],tiempo[0])) estado++;
//      break;
//
//      case 'E':
//        Serial.println("Algodon a ");
//       if( Algodon(temperatura[1],tiempo[1])) estado++;
//      break;
//
//      case 'X':
//        Serial.println("Fin del programa\n");
//        estado = 0;
//        memset(trama, 0, sizeof(trama));
//        memset(temperatura, 0, sizeof(temperatura));
//      break;
//  }
//




}
