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
int nProc = 0;                 // Variable que almacena el codigo del teñido que se esta ejecutando
int nPaso = 0;                // Varable que almacena el estado o paso del proceso. 
//int Estado_anterior = 0;      // Almacena el estado anterior para regresar a el luego de que se haya realizado el suavizado o el lavado reductivo

// Variables de interrupcion usadas en Valvulas.cpp
// Funciones encargadas de la interrupcion del sistema.
//Control_Valvulas master;
volatile int interrupt = false;     // Activa la interrupcion y para el proceso
volatile int start = false;        // Este flag espera a que el operador presione el boton de start para comenzar el proceso
int Intrr = false;                  // Produce un cambio de proceso mediante interrupcion
int Intrr_2 = false;                // Este es usado en caso de que la interrupcion ocurra dentro de lavado reductivo o suavizado

// Declaracion de objeto que representa los botones del Nextion

//NexButton bNext=NexButton(0,15,"bNext"); 
NexButton bNext=NexButton(9,3,"bNext");          // Page confirmacion
NexButton bSelec=NexButton(11,5,"bSelec");       // Page Preguntar
NexButton bBack=NexButton(11,8,"bBack");         // Page Preguntar
NexButton bRight=NexButton(8,21,"bRight");       // Page TomaMuestra 
NexButton bLeft=NexButton(8,22,"bLeft");         // Page TomaMuestra 
NexButton bOk=NexButton(14,3,"bOk");             // Page TomaMuestra 
NexButton bSuavSi=NexButton(12,2,"bSuavSi");     // Page Suavizado
NexButton bLavRedSi=NexButton(13,2,"bLavRedSi"); // Page LavReductivo
NexButton bAct=NexButton(26,27,"bAct");           // Page Gaston




// Declaracion de lista de eventos de los botones del Nextion
NexTouch *nex_listen_list[] = {
  &bNext,
  &bSelec,
  &bRight,
  &bLeft,
  &bOk,
  &bBack,
  &bSuavSi,
  &bLavRedSi,
  &bAct,
  NULL
};


/*
   Funciones que seran llamadas al producirse un evento
*/

// Recibe y desentrama los datos para iniciar un nuevo proceso de teñido.
void bNextCallback(void*ptr){

  int data_ok;

	memset(buffer, 0, sizeof(buffer));  	// Limpia el buffer para recibir la data
  memset(trama, 0, sizeof(trama)); 
  memset(temperatura, 0, sizeof(temperatura));
  memset(tiempo, 0, sizeof(tiempo));
  bNext.getText(buffer, sizeof(buffer));  // Recibe los datos y se almacenan en el buffer					
	data_ok = desentramado(trama,temperatura,tiempo);		// Separa los datos para inciar el proceso

	if(data_ok == false)
  {
    memset(buffer, 0, sizeof(buffer));    // Limpia el buffer para recibir la data
    memset(trama, 0, sizeof(trama)); 
    memset(temperatura, 0, sizeof(temperatura));
    memset(tiempo, 0, sizeof(tiempo));
  }else
  {
  // Imprime por serial los valores para ser monitoreados
	Serial.println(buffer);	
	Serial.println(trama);
  Serial.println("Preblanqueo");
	Serial.println(temperatura[0]);   // Temperatura para preblanqueos
  Serial.println(tiempo[0]);        // Tiempo para preblanqueos
  Serial.println("=============================");
  Serial.println("Poliester");
	Serial.println(temperatura[1]);   // Temperatura para el poliester
  Serial.println(tiempo[1]);        // Tiempo para el poliester
  Serial.println("==============================");
  Serial.println("Algodon");
  Serial.println(temperatura[2]);   // Temperatura para el algodon
  Serial.println(tiempo[2]);        // Tiempo para el algodon
  Serial.println(tiempo[3]);        // Tiempo para el algodon
  Serial.println("==============================");
  Serial.println("Directo");
  Serial.println(temperatura[4]);   // Temperatura para el directo
  Serial.println(tiempo[4]);        // Tiempo para el directo 
  Serial.println("==============================");
  }
}

// Funcion usada en Tomar_muestra, se usa para seleccionar un subproceso que se quiera repetir.
void bSelecCallback(void*ptr){
  
  Seleccion_proceso(nProc,nPaso);
  //Serial.println(buffer); 
  Serial.println(nProc);
  Serial.println(nPaso);
}

void bRightCallback(void*ptr){

  nPaso++;
  Seleccion_proceso(nProc,nPaso);
  Serial.println(buffer); 
  Serial.println(nProc);
  Serial.println(nPaso);
}

void bLeftCallback(void*ptr){
  
  nPaso--;
  Seleccion_proceso(nProc,nPaso);
  Serial.println(buffer); 
  Serial.println(nProc);
  Serial.println(nPaso);
}


// Funcion usada en Tomar_muestra, confirma que hay un cambio de estado.
void bCambiarEstadoCallback(void*ptr){

  Nuevo_estado = nPaso;
  Nuevo_estado_ok = true;
  Serial.println("Nuevo estado");
  Serial.println(Nuevo_estado);

}

void bBackCallback(void*ptr){
  Back = true;
}

void SuavizadoSiCallback(void*ptr){
  suav = true;
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
          Lista_Suavizado(estado);
        break;

        case 8:
          Lista_Lavado_reductivo(estado,80,30);
        break;

        case 9:
          prueba(estado,130,60);
        break;

        case 10:
          Lista_Lavado_Maquina(estado);
        break;
    }

}

// Maquinas de estados
void loop(){

	static int estado = 0;           // Esta variable recorrera los estados del switch segun lo contenido en el array trama
  static int print_code = true;    // Imprime en pantalla el codigo del proceso que se esta ejecutando solo una vez por proceso
  
  // Verifica si se reciben datos de la pantalla
  nexLoop(nex_listen_list); 
  Handler();
  
  // Interrupcion del proceso
  if(interrupt) Callback_ISR();
  

  // if (start) Test();
  // Espera a que el operador presione el boton de start para iniciar el proceso
  if(digitalRead(START) >= HIGH && !interrupt)
  {
    if(trama[estado] != 0) start =  true;
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
          nProc = 1;
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
          nProc = 2;
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
          nProc = 3;
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
          nProc = 4;
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
          nProc = 5;
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
          nProc = 6;
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
          nProc = 10;
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
        Detener_proceso();
        send_msj("nProc.val=",0);
        nProc = 0;
        send_Strmsj("page Repetir");
        estado = 0;
        start = false;
        //Reinicia los temporizadores y limpia los buffers
        Reset();                              
        memset(trama, 0, sizeof(trama));      
        memset(temperatura, 0, sizeof(temperatura));  
        memset(tiempo, 0, sizeof(tiempo));
        reiniciar_contadores();

      break;
    }
  }

}
