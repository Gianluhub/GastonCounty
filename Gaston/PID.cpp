#include <PID_AutoTune_v0.h>
#include <PID_v1.h>
#include <SPI.h>
#include <Controllino.h>
#include "procesos.h"
#include "Pines.h"




// Paramestros PID control de temperatura
double Setpoint, Input, Output;
double Kp = 10, Ki = 5, Kd = 0;
int Hist; // Histeresis de marcha, indica cuando arrancar o no el controlador
double salida = 0;
int state = RAMP_UP;  // Maquina de estados 
PID myPID(&Input,&Output,&Setpoint,Kp,Ki,Kd,DIRECT);
PID myPID2(&Input,&Output,&Setpoint,Kp,Ki,Kd,REVERSE);



// =========================================//
/* Parametros para el control de gradiente */

uint16_t maxTemp;        // Temperatura maxima deseada
float grad;              // Tasa de subida en °/min
double init_Temp;        // Temperatura inicial
double Tiempo_deseado;   // Tiempo en el que quiero que se llegue a maxTemp 
uint16_t WindowSize;     // Intervalo de tiempo en el que se se dejara en on/off la salida
unsigned long init_Time;
unsigned long init_Time2; 
int inicio = true;
bool end_soak = false;
int State_RampU = 1;
int State_Soak = 1;
int State_RampD = 1;
int Step_Ok = false;
int minTemp;
// =========================================//



// =============================================================================================================//    
/*  Parametros para 2 grados por minutos      |     Para 2 grados por min
                                              | 
    kp = 2500                                 |   kp =  2500
    ki = 0.5                                  |   ki =  0.3
    kd = 0                                    |   kd = 0
    Window = 10000                            |   Window = 10000
    error = 0.84                              |   error = 0.86
    
*/
// =============================================================================================================//



void Serial_msj();
void CoolGrad_2();
void HeatGrad_1_5();


int State_PID(int Mode, int Temp) {
  //static int mode = RAMP_UP;

  if(timerPID(100))
   {
    Serial.print("@");
    Serial.print("/");
    Serial.print(float(millis()-init_Time2)/60000);
    Serial.print("/");
    Serial.println(Temp_actual());
    //Serial.println(30);
   }


   switch(Mode)
   {
      case RAMP_UP:

        switch(State_RampU)
        {

          case 1:

            maxTemp = Temp;
            Setup_RAMP_UP();
            State_RampU++;

          break;

        case 2:

          if(maxTemp > 90 )
          { 
            if(Temp_actual() > 90)
            {
              //Serial_msj();
              HeatGrad_1_5();
              //timerPID(false);
              State_RampU = 3; 
            }
          }else State_RampU = 3;

        break;

          case 3:

            if((Temp_actual() >= maxTemp) || digitalRead(PIROMETRO) <= LOW)
            { 
              //Serial_msj();
              //timerPID(false);
              state = SOAK;
              Step_Ok = true;
              //mode = SOAK;
            }

          break;
        }

      break;

    case SOAK:

      switch(State_Soak)
      {
        case 1:

          maxTemp = Temp;
          Setup_SOAK();
          State_Soak = 2;
          
        break;

        case 2:

          if(end_soak)
          { 
            State_Soak = 1;
            Step_Ok = true;
            state = RAMP_DOWN;
            end_soak = false;
          }

        break;
      }

    break;


    case RAMP_DOWN:

      switch(State_RampD)
      {

        case 1:

          minTemp = Temp;
          maxTemp = Temp_actual();
          CoolGrad_2(); 
          State_RampD=3;
          //if (maxTemp < 100) {CoolGrad_2(); State_RampD=3;}
          //else {Setup_RAMP_DOWN(); State_RampD = 2;} 
        break;

        case 2:

          if(maxTemp >= 98)
          {
            if(Temp_actual() <= 80 )
            { 
              Serial_msj();
              CoolGrad_2();
              //timerPID(false);
              State_RampD = 3;
            }

          }else State_RampD = 3;

        break;

        case 3:

          if (Temp_actual() <= minTemp)
          {
            State_RampD = 1; 
            Step_Ok = true;
            //timerPID(false);
            //Serial_msj();           
          }

        break;
      }

    break;

   }  
  if (Step_Ok)
  { 
    //Cierra las valvulas 
    digitalWrite(FV202,LOW);
    digitalWrite(FV208,LOW);
    digitalWrite(FV209,LOW);
    digitalWrite(FV201,LOW);
    digitalWrite(FV203,LOW);
    Reset_PID();
    digitalWrite(ENFRIAMIENTO,LOW);
    digitalWrite(CALENTAMIENTO,LOW);
    Step_Ok = false;
    return true;
  }
  else PID_ON();
  return false;
}   



// float Temp_actual(){
//     float Temp = map(analogRead(TC100),129,664,-1778,14889);
//     return Temp/100;
// }

float Temp_actual(){

    float Temp = 0;
    float Temp2 = 0;
    float Temp_prom = 0;
    float i = 0;
    float t_ini = millis();
    for(i = 0; i < 500; i++)
    {
        //Temp = map(analogRead(TC100),129,664,-1778,14889)/100;
        Temp = map(analogRead(TC100),33,164,-1778,14889)/100;
        //Temp = map(analogRead(TC100),133,667,-50,150);
        //Temp = map(analogRead(TC100),33,164,-50,150);
        Temp_prom += Temp;
    }
    Temp_prom = Temp_prom/500;
    //memset(Temp_actual, 0, sizeof(Temp_actual));
    //Serial.println(analogRead(TC100));
    //Serial.println(millis()- t_ini);

    return Temp_prom;
}


void PID_ON(){

  static unsigned long Start_time = millis();     // Tiempo inicial desde que se inicia el calentamiento
  //static int state = 0;
  static int Prev_state= state;
  //uint8_t soak = 0;
  double t;                                       // tiempo de cambio de setpoint
  
  switch(state)
  {
    case RAMP_UP:
      // Por cada iteracion cambiara el setpoint
      // que representa una variacion pequeña de temperatura
      t = float(millis() - init_Time)/60000;  // Tiempo a variar en minutos
      if (inicio){t=t+1;inicio=false; Start_time = millis();}
      Input = Temp_actual();
      Setpoint = (grad*t) + init_Temp; 
      //if(Setpoint >= maxTemp) Setpoint = maxTemp;
      myPID.Compute();
      salida = Output;
      Prev_state = RAMP_UP;    
      state=RELAY;
      // Imprimir por serial
      Serial.println("============= Calentamiento ==============");
      //Serial.println("STATE: "+String(state));
      Serial.println("Setpoint: "+String(Setpoint));
      Serial.println("Input: "+String(Input));
      Serial.println("Tiempo: "+String(t));
      Serial.println("Grad: "+String(grad*t)); 
      Serial.println("Salida "+String(salida));
      Serial.println(analogRead(TC100));
      //Serial.println(float(analogRead(TC100)*20/685));
      //Serial.println(analogRead(LC100));
    
    break;

    case SOAK:

      Input = Temp_actual();
      myPID.Compute();
      salida = Output;
      Prev_state = SOAK;
      if(maxTemp == 130) state = RELAY_130;
      if(maxTemp == 60) state = RELAY_60;
      else state = RELAY;
      if(inicio) {Start_time = millis(); inicio = false;}  

      // Serial
      //Serial.println("Setpoint: "+String(Setpoint));
      Serial.println("============= SOAK ==============");
      Serial.println("STATE: "+String(state));
      Serial.println("Input: "+String(Input));
      Serial.println("Salida "+String(salida));
    break;

    case RAMP_DOWN:
      
        // Por cada iteracion cambiara el setpoin que representa una variacion pequeña de temperatura
      t = float(millis() - init_Time)/60000;  // Tiempo a variar en minutos
      if (inicio){t=t+1;inicio=false; Start_time = millis();}
      Input = Temp_actual();
      Setpoint = maxTemp - (grad*t);   
      myPID2.Compute();
      salida = Output;
      Prev_state = RAMP_DOWN;    
      state=RELAY2;

      // Imprimir por serial
      Serial.println("============= Enfriamiento ==============");
      Serial.println("STATE: "+String(state));
      Serial.println("Setpoint: "+String(Setpoint));
      Serial.println("Input: "+String(Input));
      Serial.println("Tiempo: "+String(float(millis() - init_Time)/60000));
      Serial.println("Grad: "+String(grad*t)); 
      Serial.println("Salida "+String(salida));
      //Serial.println(analogRead(TC100));
      //Serial.println(analogRead(LC100));


    break;

    case RELAY:


      //Serial.println(millis() - Start_time);
      if(salida < 3000) salida += 3000;
      if(salida > millis() - Start_time)
      {
        digitalWrite(CALENTAMIENTO,HIGH);
        digitalWrite(FV202,HIGH);
        digitalWrite(FV208,HIGH);
        FV209_on();    
      }
      else if(millis() - Start_time < WindowSize)
      {
        digitalWrite(CALENTAMIENTO,LOW);
        digitalWrite(FV202,LOW);
        digitalWrite(FV208,LOW);
        digitalWrite(FV209,LOW);        
      } 

      if(millis() - Start_time >= WindowSize)
      { 
        Start_time += WindowSize;
        state = Prev_state;
      }  
    break;

    case RELAY2:

      Serial.println("Realy 2");
      if(salida < 3000) salida += 3000;
      if(salida > millis() - Start_time)
      {
        digitalWrite(ENFRIAMIENTO,HIGH);
        digitalWrite(FV201,HIGH);
        digitalWrite(FV203,HIGH);
      }
      else if(millis() - Start_time < WindowSize)
      {
        digitalWrite(ENFRIAMIENTO,LOW);
        digitalWrite(FV201,LOW);
        digitalWrite(FV203,LOW);    
      } 
      
      if(millis() - Start_time >= WindowSize)
      { 
        Start_time += WindowSize;
        state = RAMP_DOWN;
      }  
    break;

    case RELAY_130:

      if(digitalRead(PIROMETRO) >= HIGH)
      {
        digitalWrite(CALENTAMIENTO,HIGH);
        digitalWrite(FV202,HIGH);
        digitalWrite(FV208,HIGH);
        FV209_on();
      }else
      {
        digitalWrite(CALENTAMIENTO,LOW);
        digitalWrite(FV202,LOW);
        digitalWrite(FV208,LOW);
        digitalWrite(FV209,LOW); 
        //timerPID(false);
      }

    case RELAY_60:

      if(Temp_actual() <= maxTemp - 2)
      {
        digitalWrite(CALENTAMIENTO,HIGH);
        digitalWrite(FV202,HIGH);
        digitalWrite(FV208,HIGH);
        FV209_on();
      }else if(Temp_actual() > maxTemp)
      {
        digitalWrite(CALENTAMIENTO,LOW);
        digitalWrite(FV202,LOW);
        digitalWrite(FV208,LOW);
        digitalWrite(FV209,LOW); 
        //timerPID(false);
      }

    break;
  }
 
}
 
// Usado para circulacion
int timerPID(unsigned long interval){

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

// Se usa para filtrar picos de temperatura
int timerPID_2(unsigned long interval){

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

void Setup_RAMP_UP(){

  //  Seteo de parametros inicialmente para 2°/min                      
  Kp = 3000;
  Ki = 0.6;
  Kd = 0;
  grad = 2;
  WindowSize = 10000;
  state = RAMP_UP;
  Input = Temp_actual();
  myPID.SetTunings(Kp,Ki,Kd);
  myPID.SetOutputLimits(0,WindowSize);  // Rango de la salida del PID
  myPID.SetControllerDirection(DIRECT);
  myPID.SetMode(AUTOMATIC);
  inicio = true;

  // Calculo de parametros iniciales
  init_Temp = Temp_actual();
  init_Time = millis();
  Tiempo_deseado = float(maxTemp - init_Temp)/grad;

  Serial.println("RAMP UP");

}

void Setup_SOAK()
{
  //  Seteo de parametros iniciales del PID                      
  //Kp = 2500;
  //Ki = 0.5;
  //Kd = 0;
  //grad = 2;
  WindowSize = 10000;
  state = SOAK;
  Setpoint = maxTemp;
  Input = Temp_actual();
  //myPID.SetTunings(Kp,Ki,Kd);
  myPID.SetControllerDirection(DIRECT);
  myPID.SetOutputLimits(0,WindowSize);  // Rango de la salida del PID
  myPID.SetMode(AUTOMATIC);

  Serial.println("SOAK");

  
}


void HeatGrad_1_5(){

  //  Seteo de parametros iniciales del PID                      
  Kp = 3000;
  Ki = 0.3;
  Kd = 0;
  grad = 1.5;
  WindowSize = 10000;
  Input = Temp_actual();
  myPID.SetTunings(Kp,Ki,Kd);
  myPID.SetOutputLimits(0,WindowSize);  // Rango de la salida del PID
  //myPID.SetControllerDirection(DIRECT);
  myPID.SetMode(AUTOMATIC);
  //inicio = true;

  // Calculo de parametros iniciales
  //init_Temp = Temp_actual();
  //init_Time = millis();
  //init_Time2 = millis();
  Tiempo_deseado = float(maxTemp - Temp_actual())/grad;

  Serial.println("RAMP UP");
  Serial.println("GRAD 1.5");

}




void Setup_RAMP_DOWN(){

  //  Seteo de parametros inicialmente para 2°/min                      
  Kp = 2600;
  Ki = 0.4;
  Kd = 0;
  grad = 1.5;
  WindowSize = 10000;
  state = RAMP_DOWN;
  Input = Temp_actual();
  myPID2.SetTunings(Kp,Ki,Kd);
  myPID2.SetOutputLimits(0,WindowSize);  // Rango de la salida del PID
  //myPID.SetControllerDirection(REVERSE);
  myPID2.SetMode(AUTOMATIC);
  inicio = true;
  
  // Calculo de parametros iniciales
  init_Temp = Temp_actual();
  init_Time = millis();
  init_Time2 = millis();
  Tiempo_deseado = float(init_Temp - minTemp)/grad;

  Serial.println("RAMP DOWN");


}



void CoolGrad_2(){

  //  Seteo de parametros iniciales del PID                      
  // Kp = 2500;
  // Ki = 0.5;
  // Kd = 0;
  Kp = 3000;
  Ki = 0.4;
  Kd = 0;
  grad = 2;
  WindowSize = 10000;
  state = RAMP_DOWN;
  Input = Temp_actual();
  myPID2.SetTunings(Kp,Ki,Kd);
  myPID2.SetOutputLimits(0,WindowSize);  // Rango de la salida del PID
  myPID2.SetMode(AUTOMATIC);
  inicio = true;

  // Calculo de parametros iniciales
  init_Temp = Temp_actual();
  init_Time = millis();
  init_Time2 = millis();
  Tiempo_deseado = float(init_Temp - minTemp)/grad;

  Serial.println("RAMP DOWN");
  Serial.println("GRAD 2");

}


void Serial_msj(){

    double Tiempo_obtenido;
    Tiempo_obtenido = (float(millis()-init_Time)/60000);
    Serial.println(" ========================================== ");
    Serial.println("Tiempo deseado: "+String(Tiempo_deseado));
    Serial.println("Tiempo obtenido: "+String(Tiempo_obtenido));
    Serial.println("Error: "+String(abs((Tiempo_obtenido - Tiempo_deseado))));
    Serial.println("Grad obtenido: "+String(float(maxTemp - init_Temp)/Tiempo_obtenido));
    Serial.println("kp = "+String(Kp));
    Serial.println("ki = "+String(Ki));
    Serial.println(" ========================================== ");
}


void End_Soak(){
  end_soak = true;
  State_PID(SOAK, maxTemp);

}

void Reset_PID(){

  inicio = true;
  Step_Ok = false;
  end_soak = false;
  State_RampU = 1;
  State_Soak = 1; 
  State_RampD = 1;
  timerPID(false);

}
