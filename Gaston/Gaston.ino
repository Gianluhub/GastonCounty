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
  * temperatura: Almacena la temperatura que alcanzara el tanque al iniciar el proceso de teñido.
  				  Para el poliester la temepratura se almacena en el indice 0 y para el algodon en el 1.
  * tiempo: Almacena el tiempo de circulacion despues de subir la temperatura. El indice 0 es para el poleistar y el 1 para el algodon.
  * Nuevo_estado: Se utiliza seleccionar el estado de un proceso que se quiere repetir en el evento Tomar_muestra.
  * estado_ok: Confirma que si se repetira un proceso
*/ 
char buffer[100] = {0};		
char trama[100] = {0};		
int temperatura[2] = {0};	
int tiempo[2] = {0};
// Variables para Toma de muestra
int Nuevo_estado = 0;
int Nuevo_estado_ok = false;


// Declaracion de objeto que representa los botones del Nextion
NexButton bNext=NexButton(7,1,"bNext");
NexButton bSelec=NexButton(9,1,"bSelec");
NexButton bRight=NexButton(8,21,"bRight");
NexButton bLeft=NexButton(8,22,"bLeft");
NexButton bOk=NexButton(8,27,"bOk");



// Declaracion de lista de eventos de los botones del Nextion
NexTouch *nex_listen_list[] = {
  &bNext,
  &bSelec,
  &bRight,
  &bLeft,
  &bOk,
  NULL
};


/*
   Funciones que seran llamadas al producirse un evento
*/

// Recibe y desentrama los datos para iniciar un nuevo proceso de teñido.
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

// Funcion usada en Tomar_muestar, se usa para seleccionar un subproceso que se quiera repetir.
void bSelecCallback(void*ptr){

  send_msj("nPaso.val=",30);    // Muestra en pantalla el paso del proceso
  send_msj("nProc.val=",5);  
  
  int Dato[2] = {0}; // La posicion 0 almacena el codigo del proceso y la posicion 1 el estado
  int i = 0;
  memset(buffer, 0, sizeof(buffer));    // Limpia el buffer para recibir la data
  bSelec.getText(buffer,sizeof(buffer)); // Recibe los datos y se almacenan en el buffer
  // En este caso llegan son 2 bytes de datos, el primero es el codigo del proceso y el segundo el estado donde se encuentra
  i = Tomar_Dato(i,'-',buffer,Dato,'c','e');
  i = Tomar_Dato(i,'$',buffer,Dato,'c','e');
  Seleccion_proceso(Dato[0],Dato[1]);
  Serial.println(buffer); 
  Serial.println(Dato[0]);
  Serial.println(Dato[1]);
}

void bRightCallback(void*ptr){

  int Dato[2] = {0}; // La posicion 0 almacena el codigo del proceso y la posicion 1 el estado
  int i = 0;
  memset(buffer, 0, sizeof(buffer));    // Limpia el buffer para recibir la data
  bSelec.getText(buffer,sizeof(buffer)); // Recibe los datos y se almacenan en el buffer
  // En este caso llegan son 2 bytes de datos, el primero es el codigo del proceso y el segundo el estado donde se encuentra
  i = Tomar_Dato(i,'-',buffer,Dato,'c','e');
  i = Tomar_Dato(i,'$',buffer,Dato,'c','e');
  Seleccion_proceso(Dato[0],Dato[1]);
  Serial.println(buffer); 
  Serial.println(Dato[0]);
  Serial.println(Dato[1]);
}

void bLeftCallback(void*ptr){
  
  int Dato[2] = {0}; // La posicion 0 almacena el codigo del proceso y la posicion 1 el estado
  int i = 0;
  memset(buffer, 0, sizeof(buffer));    // Limpia el buffer para recibir la data
  bSelec.getText(buffer,sizeof(buffer)); // Recibe los datos y se almacenan en el buffer
  // En este caso llegan son 2 bytes de datos, el primero es el codigo del proceso y el segundo el estado donde se encuentra
  i = Tomar_Dato(i,'-',buffer,Dato,'c','e');
  i = Tomar_Dato(i,'$',buffer,Dato,'c','e');
  Seleccion_proceso(Dato[0],Dato[1]);
  Serial.println(buffer); 
  Serial.println(Dato[0]);
  Serial.println(Dato[1]);
}


// Funcion usada en Tomar_muestra, confirma que hay un cambio de estado.
void bCambiarEstadoCallback(void*ptr){

  int Dato[2] = {0};
  int i = 0;
  memset(buffer, 0, sizeof(buffer));    // Limpia el buffer para recibir la data
  bOk.getText(buffer,sizeof(buffer));   // Recibe los datos y se almacenan en el buffer
  i = Tomar_Dato(i,'$',buffer,Dato,'c','e');
  Nuevo_estado = Dato[1];
  Nuevo_estado_ok = true;

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
    i = Tomar_Dato(i,'-',buffer,temperatura,'p','a');
    i = Tomar_Dato(i,'#',buffer,tiempo,'p','a');
    
 
  }while(buffer[i]!='X');
  trama[j] = 'X';
}


/*
  Extrae los datos de interes al detectar un caracter de inicio y guarda
  los datos en formato entero.
*/
int Tomar_Dato(int i,char start, char buffer[], int save[2],char save1, char save2){

  int k = 0;  // Indice de aux
  char aux[4] = {0};  // Array auxiliar
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
    if(aux2 == save1) save[0] = String(aux).toInt();  // Convierte los caracteres a un entero y se guarda    
  
    // Si la temperatura o el tiempo pertenece a la del algodon guarda ese valor en la posicion 1 del array
    else if(aux2 == save2) save[1] = String(aux).toInt();  // Convierte los caracteres a un entero y se guarda    
  }
  return i;

}


// Funciones para envio de datos a la pantalla 
void send_msj(char msj[],int dato){

  Serial2.print(msj);  // Se envia un string que hace referencia a la variable del HMI que se desea modificar ejemplo "nTemp.val="
  Serial2.print(dato);  // Este es el dato que se guardara en la variable msj
  Serial2.write(0xff);  // Indica fin de la trama
  Serial2.write(0xff);
  Serial2.write(0xff);
}

// Envia un String de datos a la pantalla
void send_Strmsj(char msj[]){

  Serial2.print(msj);  // Se envia un string que hace referencia a la variable del HMI que se desea modificar ejemplo "nTemp.val="
  Serial2.write(0xff);  // Indica fin de la trama
  Serial2.write(0xff);
  Serial2.write(0xff);
}


// Envia informacion de interes a la pantalla sobre el proceso que esta ejecutandose
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

// Actualiza el tiempo restante en la pantalla cada minuto
// Utiliza timer4 y el tiempo debe ser en minutos
// Es obligatorio reiniciar el contador despues de usar la funcion
void Act_tiempo(int tiempo){

  static int cont = 0; // Contador que llevara el tiempo restante

  if (timer4(60000))
  {
    cont++;
    send_msj("nTiempo.val=",tiempo-cont);
    send_msj("nTempA.val=",Temp_actual());
  }

  // Reinicia el contador
  if(tiempo == false)
  {
    cont = 0;
    timer4(false);
    send_msj("nTiempo.val=",0); 
  }
}

// Permite seleccionar nuevamente un proceso
void Seleccion_proceso(int codigo, int estado){

    switch(codigo)
    {
        case 1:
          Lista_preblanqueo_quimico(estado);
        break;

        case 2:
          Lista_preblanqueo_jabon(estado);
        break;

        case 3:
          Lista_Saponizado(estado);
        break;

        case 4:
          Lista_Poliester(estado,temperatura[0],tiempo[0]);
        break;

        case 5:
          Lista_Algodon(estado,temperatura[1],tiempo[1]);
        break;

        case 6:
          prueba(estado);
        break;
    }

}

// Maquinas de estados
void loop(){

	static int estado = 0;   // Esta variable recorrera los estados del switch segun lo contenido en el array trama
  static int flag = true;
	nexLoop(nex_listen_list); // Verifica si se reciben datos del HMI
  int temp_ok = 0;  
  int pi_ok = 0;

  static int check_state = 1;     // Se uitiliza para actualizar en la pantalla el paso del proceso
  if (check_state != estado)
  {
    send_msj("nPaso.val=",estado);    // Muestra en pantalla el paso del proceso
    send_msj("nProc.val=",6);
    check_state = estado;
  }

  switch (estado) {

        case 0:
          //Serial.println("case 0");
         // send_msj("nProc.val=",4);
         // send_msj("nPaso.val=",1);
          if(digitalRead(Start)>=HIGH) estado=11;  
        break;

        case 1:
          if(Llenado(1)) estado ++;
        break;

        case 2:
          if(Llenado(2)) estado++;
        break;

        case 3:
          if(Llamado_op()) estado++;
        break;

        case 4:
          if(Adicion_rapida(2)) estado++;
        break;

        case 5:
          if(Adicion_lenta(1,2,2)) estado++;
        break;

        case 6:
          if(Circulacion(1)) estado++;
        break;

        case 7:
          if(Lavado_rebose(1)) estado++;
        break;

        case 8:
          if(Vaciado()) estado++;
        break;

        case 9:
          temp_ok = Calentamiento(130,2);
          pi_ok = Presurizado();
          Serial.println(Temp_actual());
          if(Temp_actual() >= 150 && temp_ok && pi_ok) estado++;
        break;

        case 10:
          Serial.println(Temp_actual());
          temp_ok = Enfriamiento(60,2);
          pi_ok = Despresurizado();
          if(Temp_actual() <= 60 && temp_ok && pi_ok) estado++;
        break;

        case 11:
          estado = Tomar_muestra(estado);

        break;

        case 12:
          Serial.println("Fin de programa");
          flag = true;
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
//        if(Preblanqueo_quimico()) flag = true;
//      break;
//
//      case 'B':
//
//        Serial.println("Preblanqueo con jabon\n");
//        
//        flag = true;
//      break;
//
//      case 'C':
//        
//      break;
//
//
//      case 'D':
//        Serial.println("Poliester a ");
//        if(Poliester(temperatura[0],tiempo[0])) flag = true;
//      break;
//
//      case 'E':
//        Serial.println("Algodon a ");
//       if( Algodon(temperatura[1],tiempo[1])) flag = true;
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
