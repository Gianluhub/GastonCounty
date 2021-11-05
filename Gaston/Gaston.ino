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
  
  // int Dato[2] = {0}; // La posicion 0 almacena el codigo del proceso y la posicion 1 el estado
  // int i = 0;
  // nProc = 0;
  // nPaso = 0;
  // memset(buffer, 0, sizeof(buffer));    // Limpia el buffer para recibir la data
  // bSelec.getText(buffer,sizeof(buffer)); // Recibe los datos y se almacenan en el buffer
  // // En este caso llegan son 2 bytes de datos, el primero es el codigo del proceso y el segundo el estado donde se encuentra
  // i = Tomar_Dato(i,'-',buffer,Dato);
  // i = Tomar_Dato(i,'$',buffer,Dato);
  // nProc = Dato[0];
  // nPaso = Dato[1];
  Seleccion_proceso(nProc,nPaso);
  //Serial.println(buffer); 
  Serial.println(nProc);
  Serial.println(nPaso);
}

void bRightCallback(void*ptr){

  // int Dato[2] = {0}; // La posicion 0 almacena el codigo del proceso y la posicion 1 el estado
  // int i = 0;
  // memset(buffer, 0, sizeof(buffer));    // Limpia el buffer para recibir la data
  // bSelec.getText(buffer,sizeof(buffer)); // Recibe los datos y se almacenan en el buffer
  // // En este caso llegan son 2 bytes de datos, el primero es el codigo del proceso y el segundo el estado donde se encuentra
  // i = Tomar_Dato(i,'-',buffer,Dato);
  // i = Tomar_Dato(i,'$',buffer,Dato);
  nPaso++;
  Seleccion_proceso(nProc,nPaso);
  Serial.println(buffer); 
  Serial.println(nProc);
  Serial.println(nPaso);
}

void bLeftCallback(void*ptr){
  
  // int Dato[2] = {0}; // La posicion 0 almacena el codigo del proceso y la posicion 1 el estado
  // int i = 0;
  // memset(buffer, 0, sizeof(buffer));    // Limpia el buffer para recibir la data
  // bSelec.getText(buffer,sizeof(buffer)); // Recibe los datos y se almacenan en el buffer
  // // En este caso llegan son 2 bytes de datos, el primero es el codigo del proceso y el segundo el estado donde se encuentra
  // i = Tomar_Dato(i,'-',buffer,Dato);
  // i = Tomar_Dato(i,'$',buffer,Dato);
  nPaso--;
  Seleccion_proceso(nProc,nPaso);
  Serial.println(buffer); 
  Serial.println(nProc);
  Serial.println(nPaso);
}


// Funcion usada en Tomar_muestra, confirma que hay un cambio de estado.
void bCambiarEstadoCallback(void*ptr){

  // int Dato[2] = {0};
  // int i = 0;
  // memset(buffer, 0, sizeof(buffer));    // Limpia el buffer para recibir la data
  // bOk.getText(buffer,sizeof(buffer));   // Recibe los datos y se almacenan en el buffer
  // i = Tomar_Dato(i,'$',buffer,Dato);
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
  int testing = false;
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
  

  if(start && !testing)
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

      break;
    }
  }

}


/*===========================================================================================================================*/
/*===========================================================================================================================*/
/*===========================================================================================================================*/
/*===========================================================================================================================*/
//                      FUNCIONES DE PRUEBA
void Test(){

  static int estado = 0;   // Esta variable recorrera los estados del switch segun lo contenido en el array trama
  static int Estado_anterior = 0;
  //nexLoop(nex_listen_list); // Verifica si se reciben datos del HMI
  int temp_ok = 0;  
  int pi_ok = 0;

  if(Intrr)
  {
    Intrr = false;
    estado = Nuevo_estado;
  }

  static int check_state = 1;     // Se uitiliza para actualizar en la pantalla el paso del proceso
  if (check_state != estado)
  {
    send_msj("Proceso.nPaso.val=",estado);    // Muestra en pantalla el paso del proceso
    send_msj("Proceso.nProc.val=",9);
    nProc = 9;
    //Serial.println(estado);
    check_state = estado;
    nPaso = estado;
    Serial.println("Estado"+String(estado));
    if(estado == 0) Serial.println("Inicio");
  }

  if(Next()) estado++;
  
  switch (estado) {

        case 0:          
          if(digitalRead(START)>=HIGH)
          { 
           estado=1;  
          } 
        break;

        case 1:
          if(Llenado(1)) estado=2;
        break;

        case 2:
          if(Llenado(2)) estado=3;
        break;

        case 3:
          if(Llamado_op()) estado=4;
        break;

        case 4:
          if(Adicion_rapida(2)) estado=5;
        break;

        case 5:
          
            if(Adicion_lenta(2,1,2)) estado=6;
        break;

        case 7:
            if(Lavado_rebose(1)) estado=8;
        break;

        case 6:
         
          if(Circulacion(1)) estado=7;
        break;

        case 8:
          if(Vaciado()) estado=11;
        break;

        case 9:
          temp_ok = Calentamiento(130,2);
          pi_ok = Presurizado();
          Serial.println(Temp_actual());
          //if(Temp_actual() >= 150 && temp_ok && pi_ok) estado++;
          if(digitalRead(Op_ok)>= HIGH) estado++;
        break;

        case 10:
          Serial.println(Temp_actual());
          temp_ok = Enfriamiento(60,2);
          pi_ok = Despresurizado();
          //if(Temp_actual() <= 60 && temp_ok && pi_ok) estado++;
          if(digitalRead(Op_ok)>= HIGH) estado++;
        break;

        case 11:
          Estado_anterior = 11;
          estado = Tomar_muestra(estado);

        break;

        case 12:
          Fin_proceso();
          //send_msj("nProc.val=",0);
          //send_Strmsj("page Repetir");
          estado = 0;
          Reset();
          memset(trama, 0, sizeof(trama));
          memset(temperatura, 0, sizeof(temperatura));
          memset(tiempo, 0, sizeof(tiempo));
        break;

          // 100. Suavizado
      case 100:
        if(Suavizado()) estado = Estado_anterior;
      break;
  
      // 101. Lavado reductivo
      case 101:
        if(Lavado_reductivo(80,30)) estado = Estado_anterior;
      break;
    }
}


void bActCallback(void*ptr){

 Statechange(FV200,!digitalRead(FV200),"FV200.val=");
 Statechange(FV201,!digitalRead(FV201),"FV201.val=");
 Statechange(FV202,!digitalRead(FV202),"FV202.val=");
 Statechange(FV203,!digitalRead(FV203),"FV203.val=");
 Statechange(FV204,!digitalRead(FV204),"FV204.val=");
 Statechange(FV205,!digitalRead(FV205),"FV205.val=");
 Statechange(FV206,!digitalRead(FV206),"FV206.val=");
 Statechange(FV207,!digitalRead(FV207),"FV207.val=");
 Statechange(FV208,!digitalRead(FV208),"FV208.val=");
 Statechange(FV209,!digitalRead(FV209),"FV209.val=");
 Statechange(FV210,!digitalRead(FV210),"FV210.val=");
 Statechange(FV211,!digitalRead(FV211),"FV211.val=");
 Statechange(FV212,!digitalRead(FV212),"FV212.val=");
 Statechange(FV213,!digitalRead(FV213),"FV213.val=");
 Statechange(FV214,!digitalRead(FV214),"FV214.val=");
 Statechange(pump,!digitalRead(pump),"Pump.val=");
 Statechange(jet_1,!digitalRead(jet_1),"Jet.val=");
 Statechange(plegador_1,!digitalRead(plegador_1),"Plegador.val=");
 Statechange(TC100,!digitalRead(TC100),"TC100.val=");
 Statechange(LC100,!digitalRead(LC100),"LC100.val=");
 Statechange(LC101,!digitalRead(LC101),"LC101.val=");
 Statechange(PCH100,!digitalRead(PCH100),"PCH100.val=");
 Statechange(PCL100,!digitalRead(PCL100),"PCL100.val=");


}

void Handler(){

 static int state_FV200 = digitalRead(FV200);
 static int state_FV201 = digitalRead(FV201);
 static int state_FV202 = digitalRead(FV202);
 static int state_FV203 = digitalRead(FV203);
 static int state_FV204 = digitalRead(FV204);
 static int state_FV205 = digitalRead(FV205);
 static int state_FV206 = digitalRead(FV206);
 static int state_FV207 = digitalRead(FV207);
 static int state_FV208 = digitalRead(FV208);
 static int state_FV209 = digitalRead(FV209);
 static int state_FV210 = digitalRead(FV210);
 static int state_FV211 = digitalRead(FV211);
 static int state_FV212 = digitalRead(FV212);
 static int state_FV213 = digitalRead(FV213);
 static int state_FV214 = digitalRead(FV214);
 static int state_pump = digitalRead(pump);
 static int state_jet_1 = digitalRead(jet_1);
 static int state_plegador_1 = digitalRead(plegador_1);
 static int state_TC100 = digitalRead(TC100);
 static int state_LC100 = digitalRead(LC100);
 static int state_LC101 = digitalRead(LC101);
 static int state_PCH100 = digitalRead(PCH100);
 static int state_PCL100 = digitalRead(PCL100);
 
 state_FV200 = Statechange(FV200,state_FV200,"FV200.val=");
 state_FV201 = Statechange(FV201,state_FV201,"FV201.val=");
 state_FV202 = Statechange(FV202,state_FV202,"FV202.val=");
 state_FV203 = Statechange(FV203,state_FV203,"FV203.val=");
 state_FV204 = Statechange(FV204,state_FV204,"FV204.val=");
 state_FV205 = Statechange(FV205,state_FV205,"FV205.val=");
 state_FV206 = Statechange(FV206,state_FV206,"FV206.val=");
 state_FV207 = Statechange(FV207,state_FV207,"FV207.val=");
 state_FV208 = Statechange(FV208,state_FV208,"FV208.val=");
 state_FV209 = Statechange(FV209,state_FV209,"FV209.val=");
 state_FV210 = Statechange(FV210,state_FV210,"FV210.val=");
 state_FV211 = Statechange(FV211,state_FV211,"FV211.val=");
 state_FV212 = Statechange(FV212,state_FV212,"FV212.val=");
 state_FV213 = Statechange(FV213,state_FV213,"FV213.val=");
 state_FV214 = Statechange(FV214,state_FV214,"FV214.val=");
 state_pump = Statechange(pump,state_pump,"Pump.val=");
 state_jet_1 = Statechange(jet_1,state_jet_1,"Jet.val=");
 state_plegador_1 = Statechange(plegador_1,state_plegador_1,"Plegador.val=");
 state_TC100 = Statechange(TC100,state_TC100,"TC100.val=");
 state_LC100 = Statechange(LC100,state_LC100,"LC100.val=");
 state_LC101 = Statechange(LC101,state_LC101,"LC101.val=");
 state_PCH100 = Statechange(PCH100,state_PCH100,"PCH100.val=");
 state_PCL100 = Statechange(PCL100,state_PCL100,"PCL100.val=");

}


int Statechange(int pin, int state, char msj[]){

  
  if(digitalRead(pin)!=state){


    if(digitalRead(pin) >= HIGH)
    {
      send_msj(msj,1);
      state=HIGH;
    }
    else
    {
     send_msj(msj,0);
     state=LOW;
    }
  }
  return state;
  

}

extern int Mostrar;
int Next(){

  static int state = LOW;
  
  if(digitalRead(NEXT)!=state){
    
    Reset();
    if(digitalRead(NEXT) >= HIGH)
    {
      state=HIGH;
      Mostrar = true;
      return true;
    }
    else
    {
     state=LOW;
    }
  }
  return false;

}

int Next2(){

  static int state = LOW;
  
  if(digitalRead(NEXT2)!=state){
    
    Reset();
    if(digitalRead(NEXT2) >= HIGH)
    {
      state=HIGH;
      Mostrar = true;
      return true;
    }
    else
    {
     state=LOW;
    }
  }
  return false;

}
