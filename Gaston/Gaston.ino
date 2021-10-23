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
int temperatura[10] = {0};	
int tiempo[10] = {0};

// Variables para Toma de muestra usadas en el archivo tomar_muestra.cpp
int Nuevo_estado = 0;         // Si se desea repetir un proceso, esta variable guarda el estado a donde se quiere regresar
int Nuevo_estado_ok = false;  // Este flag funciona para indicar que se va a repetir un proceso
int Back = false;             // Si no se desea repetir algun proceso o realizar suavizado, este flag indicara que continue el programa
int suav = false;             // Flag para indicar que se realizara suavizado
int lav_red = false;          // Flag para indicar Lavado reductivo

// Declaracion de objeto que representa los botones del Nextion
NexButton bNext=NexButton(7,1,"bNext");        // Boton de confirmacion
//NexButton bNext=NexButton(0,16,"bNext"); 
NexButton bSelec=NexButton(9,1,"bSelec");
NexButton bBack=NexButton(9,2,"bBack");
NexButton bRight=NexButton(8,21,"bRight");
NexButton bLeft=NexButton(8,22,"bLeft");
NexButton bOk=NexButton(8,26,"bOk");
NexButton bSuavSi=NexButton(12,1,"bSuavSi");
NexButton bSuavNo=NexButton(12,2,"bSuavNo");
NexButton bLavRedSi=NexButton(12,1,"bLavRedSi");
NexButton bLavRedNo=NexButton(12,2,"bLavRedNo");




// Declaracion de lista de eventos de los botones del Nextion
NexTouch *nex_listen_list[] = {
  &bNext,
  &bSelec,
  &bRight,
  &bLeft,
  &bOk,
  &bBack,
  &bSuavSi,
  &bSuavNo,
  &bLavRedsi,
  &bLavRedNo,
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
	Serial.println(temperatura[0]);   // Temperatura para preblanqueos
  Serial.println(tiempo[0]);        // Tiempo para preblanqueos
	Serial.println(temperatura[1]);   // Temperatura para el poliester
  Serial.println(tiempo[1]);        // Tiempo para el poliester
  Serial.println(temperatura[2]);   // Temperatura para el algodon
  Serial.println(tiempo[2]);        // Tiempo para el algodon
  Serial.println(tiempo[3]);        // Tiempo para el algodon
  Serial.println(temperatura[4]);   // Temperatura para el directo
  Serial.println(tiempo[4]);        // Tiempo para el directo 

}

// Funcion usada en Tomar_muestar, se usa para seleccionar un subproceso que se quiera repetir.
void bSelecCallback(void*ptr){
  
  int Dato[2] = {0}; // La posicion 0 almacena el codigo del proceso y la posicion 1 el estado
  int i = 0;
  memset(buffer, 0, sizeof(buffer));    // Limpia el buffer para recibir la data
  bSelec.getText(buffer,sizeof(buffer)); // Recibe los datos y se almacenan en el buffer
  // En este caso llegan son 2 bytes de datos, el primero es el codigo del proceso y el segundo el estado donde se encuentra
  i = Tomar_Dato(i,'-',buffer,Dato);
  i = Tomar_Dato(i,'$',buffer,Dato);
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
  i = Tomar_Dato(i,'-',buffer,Dato);
  i = Tomar_Dato(i,'$',buffer,Dato);
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
  i = Tomar_Dato(i,'-',buffer,Dato);
  i = Tomar_Dato(i,'$',buffer,Dato);
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
  i = Tomar_Dato(i,'$',buffer,Dato);
  Nuevo_estado = Dato[1];
  Nuevo_estado_ok = true;

}

void bBackCallback(void*ptr){
  Back = true;
}

void SuavizadoSiCallback(void*ptr){
  suav = true;
}

void SuavizadoNoCallback(void*ptr){
  Back = true;
}

void LavadoRedSiCallback(void*ptr){
  lav_red = true;
}



// Funciones para envio de datos a la pantalla 
void send_msj(char msj[],int dato){

  Serial2.print(msj);   // Se envia un string que hace referencia a la variable del HMI que se desea modificar ejemplo "nTemp.val="
  Serial2.print(dato);  // Este es el dato que se guardara en la variable msj
  Serial2.write(0xff);  // Indica fin de la trama
  Serial2.write(0xff);
  Serial2.write(0xff);
}

// Envia un String de datos a la pantalla
void send_Strmsj(char msj[]){

  Serial2.print(msj);   // Se envia un string que hace referencia a la variable del HMI que se desea modificar ejemplo "nTemp.val="
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

  // Cada vez que pase 1 minuto restara el tiempo en la pantalla
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

// Permite seleccionar manualmente un proceso
void Seleccion_proceso(int codigo, int estado){

    switch(codigo)
    {
        case 1:
          Lista_preblanqueo_quimico(estado,temperatura[0],tiempo[0]);
        break;

        case 2:
          Lista_preblanqueo_jabon(estado,temperatura[0],tiempo[0]);
        break;

        case 3:
          Lista_Saponizado(estado,temperatura[0],tiempo[0]);
        break;

        case 4:
          Lista_Poliester(estado,temperatura[1],tiempo[1]);
        break;

        case 5:
          Lista_Algodon(estado,temperatura[2],tiempo[2],tiempo[3]);
        break;

        case 6:
          Lista_Directo(estado,temperatura[4],tiempo[4]);
        break;

        case 7:
         // Lista_Suavizado()
        break;

        case 8:
          Lista_Lavado_reductivo(estado,80,30);
        break;

        case 9:
          prueba();
        break;
    }

}

// Maquinas de estados
void loop(){

	static int estado = 0;           // Esta variable recorrera los estados del switch segun lo contenido en el array trama
  static int start = false;        // Este flag espera a que el operador presione el boton de start para comenzar el proceso
  static int print_code = true;    // Imprime en pantalla el codigo del proceso que se esta ejecutando solo una vez por proceso

  // Verifica si se reciben datos de la pantalla
  nexLoop(nex_listen_list); 

  // Espera a que el operador presione el boton de start para iniciar el proceso
  if(digitalRead(START) >= HIGH)
  {
    start =  true;
  }
  

  if(start)
  {

    switch (trama[estado]) {

      // Preblanqueo quimico
      case 'A':
        
        // Imprime solo una vez
        if(print_code)
        {
          Serial.println("Preblanqueo quimico\n");
          send_msj("nProc.val=",1);
          print_code = false;
        }

        if(Preblanqueo_quimico(temperatura[0],tiempo[0])) 
        {
          print_code = true;
          estado++;
        }

      break;

      // Preblanqueo con jabon
      case 'B':

        // Imprime solo una vez
        if(print_code)
        {
          Serial.println("Preblanqueo con jabon\n");
          send_msj("nProc.val=",2);
          print_code = false;
        }

        if(Preblanqueo_jabon(temperatura[0],tiempo[0]))
        {
          print_code = true;
          estado++;
        }

      break;

      // Saponizado
      case 'C':

        // Imprime solo una vez
        if(print_code)
        {
          Serial.println("Sanponizado\n");
          send_msj("nProc.val=",3);
          print_code = false;
        }

        if(Saponizado(temperatura[0],tiempo[0]))
        {
          print_code = true;
          estado++;
        }

      break;

      // Poliester
      case 'D':

        // Imprime solo una vez
        if(print_code)
        {
          Serial.println("Poliester");
          send_msj("nProc.val=",4);
          print_code = false;
        }
        
        if(Poliester(temperatura[1],tiempo[1]))
        {
          print_code = true;
          estado++;
        } 

      break;


      // Algodon
      case 'E':

        // Imprime solo una vez
        if(print_code)
        {
          Serial.println("Algodon");
          send_msj("nProc.val=",5);
          print_code = false;
        }
        
       if(Algodon(temperatura[2],tiempo[2],tiempo[3]))
       {
        print_code = true;
        estado++;
       } 

      break;

      // Directo
      case 'F':

        // Imprime solo una vez
        if(print_code)
        {
          Serial.println("Directo");
          send_msj("nProc.val=",6);
          print_code = false;
        }
        if( Directo(temperatura[4],tiempo[4]))
        {
          print_code = true;
          estado++;
        }

      break;

      // Lavado de maquina
      case 'G':
        // Imprime solo una vez
        if(print_code)
        {
          Serial.println("Lavado de maquina");
          send_msj("nProc.val=",7);
          print_code = false;
        }
        if(Lavado_Maquina())
        {
          print_code = true;
          estado++;
        }

      break;

      // Fin del proceso
      case 'X':

        Fin_proceso();
        send_msj("nProc.val=",0);
        estado = 0;
        start = false;
        memset(trama, 0, sizeof(trama));
        memset(temperatura, 0, sizeof(temperatura));

      break;
    }
  }

}



void Test(){

  static int estado = 0;   // Esta variable recorrera los estados del switch segun lo contenido en el array trama
  
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
        
          if(digitalRead(START)>=HIGH)
          { 
           send_msj("nProc.val=",6);
           estado=11;
              
          } 
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
          estado = 0;
        break;
       
    }
}
