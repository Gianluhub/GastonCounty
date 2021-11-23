#include <Arduino.h>
#include <PID_v1.h>
#include "PID.h"


//#define RAMP_UP 1
//#define SOAK 2
//#define RAMP_DOWN 3
//#define RELAY 4
//#define RESET 0


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



// Paramestros iniciales del PID 
double Setpoint, Input, Output;
double Kp, Ki, Kd;
int Hist; // Histeresis de marcha, indica cuando arrancar o no el controlador
int state = RAMP_UP;  // Maquina de estados 
int Prev_state= state;
PID myPID(&Input,&Output,&Setpoint,Kp,Ki,Kd,DIRECT);

// =========================================//
/* Parametros para el control de gradiente */

uint16_t maxTemp;   	// Temperatura maxima deseada
unsigned long time;		// Tiempo de circulacion
float grad;       		// Tasa de subida en °/min
double init_Temp;    	// Temperatura inicial
double Tiempo_deseado;  // Tiempo en el que quiero que se llegue a maxTemp 
uint16_t WindowSize; 	// Intervalo de tiempo en el que se se dejara en on/off la salida
unsigned long init_Time; // Tiempo inicial al momento de llamar la funcion
bool start = true;		  // Usado para reiniciar el contador cuando ocurra un nuevo ciclo de rammpa
bool Step_Ok = false;	  // Cierra todas las valvulas y retorna true
bool end_soak = false;
// =========================================//

// Comunicacion Serial
unsigned long init_Time2; // Tiempo inicial cuando se cambia el gradiente a 1.5 (solo para mostrar por serial)
double init_Temp2;    	// Temperatura inicial (solo para mostrar por serial)

// Control de maquina de estados
int State_RampU = 1;
int State_Soak = 1;
int State_RampD = 1;

int timerPID(unsigned long interval);
void Serial_msj();
float Temp_actual();
void Heat_on(unsigned long Start_time);
void Cool_on(unsigned long Start_time);
void PID_ON();


void Setup_RAMP_UP(){

	//  Seteo de parametros inicialmente para 2°/min                      
	Kp = 2500;
	Ki = 0.5;
	Kd = 0;
	grad = 2;
	WindowSize = 10000;
	state = RAMP_UP;
	Input = Temp_actual();
	myPID.SetTunings(Kp,Ki,Kd);
	myPID.SetOutputLimits(0,WindowSize);  // Rango de la salida del PID
	myPID.SetControllerDirection(DIRECT);
	myPID.SetMode(AUTOMATIC);
	start = true;

	// Calculo de parametros iniciales
	init_Temp = Temp_actual();
	init_Temp2 = Temp_actual();
	init_Time = millis();
	init_Time2 = millis();
	Tiempo_deseado = float(maxTemp - init_Temp2)/grad;

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
	//Input = Temp_actual();
	//myPID.SetTunings(Kp,Ki,Kd);
	myPID.SetControllerDirection(DIRECT);
	myPID.SetOutputLimits(0,WindowSize);  // Rango de la salida del PID
	myPID.SetMode(AUTOMATIC);

	Serial.println("SOAK");

	
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
	myPID.SetTunings(Kp,Ki,Kd);
	myPID.SetOutputLimits(0,WindowSize);  // Rango de la salida del PID
	myPID.SetControllerDirection(REVERSE);
	myPID.SetMode(AUTOMATIC);
	
	// Calculo de parametros iniciales
	//init_Temp = Temp_actual();
	init_Temp2 = Temp_actual();
	//init_Time = millis();
	init_Time2 = millis();
	Tiempo_deseado = float(maxTemp - init_Temp2)/grad;

	Serial.println("RAMP DOWN");


}

void HeatGrad_1_5(){

	//  Seteo de parametros iniciales del PID                      
	Kp = 2600;
	Ki = 0.4;
	Kd = 0;
	grad = 1.5;
	WindowSize = 10000;
	Input = Temp_actual();
	myPID.SetTunings(Kp,Ki,Kd);
	myPID.SetOutputLimits(0,WindowSize);  // Rango de la salida del PID
	myPID.SetControllerDirection(DIRECT);
	myPID.SetMode(AUTOMATIC);
	//inicio = true;

	// Calculo de parametros iniciales
	init_Temp2 = Temp_actual();
	init_Time2 = millis();
	Tiempo_deseado = float(maxTemp - init_Temp2)/grad;

	Serial.println("GRAD 1.5");

}


void CoolGrad_2(){

	//  Seteo de parametros iniciales del PID                      
	Kp = 2500;
	Ki = 0.5;
	Kd = 0;
	grad = 2;
	WindowSize = 10000;
	Input = Temp_actual();
	myPID.SetTunings(Kp,Ki,Kd);
	myPID.SetOutputLimits(0,WindowSize);  // Rango de la salida del PID
	myPID.SetControllerDirection(REVERSE);
	myPID.SetMode(AUTOMATIC);
	//inicio = true;

	// Calculo de parametros iniciales
	init_Temp2 = Temp_actual();
	init_Time2 = millis();
	Tiempo_deseado = float(maxTemp - init_Temp2)/grad;

	Serial.println("GRAD 1.5");

}



int State_PID(int Mode, int Temp, unsigned long Time){



	if(timerPID(100))
	{
		Serial.print("t");
		Serial.print("/");
		Serial.print(float(millis()-init_Time)/60000);
		Serial.print("/");
		Serial.println(Temp_actual());
	}


	switch (Mode)
	{

		case RAMP_UP:

			
			switch (State_RampU)
			{
				case 1:

					maxTemp = Temp;
					time = Time;
					Setup_RAMP_UP();
					State_RampU = 2;

				break;

				case 2:

					if(maxTemp > 100)
					{
						if(Temp_actual() >= 90)
						{
							HeatGrad_1_5();
							State_RampU = 3; 
						}
					}else State_RampU = 3;

				break;

				case 3:

					if (Temp_actual() >= maxTemp)
					{
						State_RampU = 1; 
						Step_Ok = true;
						Serial_msj();						
					}

				break;
			}

		break;

		case SOAK:

			switch(State_Soak)
			{
				case 1:

					maxTemp = Temp;
					time = Time;
					Setup_SOAK();
					//Timer2(time);
					State_Soak = 2;
					
				break;

				case 2:

					if(end_soak)
					{	
						State_Soak = 1;
						Step_Ok = true;
						end_soak = false;
					}

				break;
			}

		break;

		case RAMP_DOWN:

			switch(State_RampD)
			{

				case 1:

					maxTemp = Temp;
					time = Time;
					Setup_RAMP_DOWN();
					State_RampD = 2;

				break;

				case 2:

					if(maxTemp > 100)
					{
						if(Temp_actual() <= 80)
						{
							CoolGrad_2();
							State_RampD = 3;
						}

					}else State_RampD = 3;

				break;

				case 3:

					if (Temp_actual() <= 60)
					{
						State_RampD = 1; 
						Step_Ok = true;
						start = true;
						Reset_process();
						Serial_msj();						
					}

				break;
			}

		break;

	}

	if (Step_Ok)
	{ 
		
		// Cierra las valvulas durante el tiempo restante
        digitalWrite(FV202,LOW);
        digitalWrite(FV208,LOW);
        digitalWrite(FV209,LOW);
		digitalWrite(FV201,LOW);
        digitalWrite(FV203,LOW);
        Step_Ok = false;
		return true;

	}
	else PID_ON();
	return false;
}




void PID_ON(){

	static unsigned long Start_time = millis();     // Tiempo inicial desde que se inicia el calentamiento
	double t;                                       // tiempo de cambio de setpoint
	
	if(start) Start_time = millis();				// Resetea Start_time
	
	t = float(millis() - init_Time)/60000;  		// Tiempo a variar en minutos
	if (start){t=t+1;start=false;}

	
	switch(state)
	{
		case RAMP_UP:

			// Por cada iteracion cambiara el setpoint que representa una variacion pequeña de temperatura
			Input = Temp_actual();
			Setpoint = (grad*t) + init_Temp; 
			myPID.Compute();
			Prev_state = RAMP_UP;    
			state=RELAY;

			// Imprimir por serial
			Serial.println("Setpoint: "+String(Setpoint));
			Serial.println("Input: "+String(Input));
			Serial.println("Tiempo: "+String(t));
			Serial.println("Grad: "+String(grad*t)); 
			Serial.println("Salida "+String(Output));

		break;

		case SOAK:

			Setpoint = maxTemp;
			Input = Temp_actual();
			myPID.Compute();
			Prev_state = SOAK;
			state = RELAY;

			// Serial
			Serial.println("Setpoint: "+String(Setpoint));
			Serial.println("Input: "+String(Input));
			Serial.println("Salida "+String(Output));

		break;

		case RAMP_DOWN:

		    // Por cada iteracion cambiara el setpoin que representa una variacion pequeña de temperatura
			Input = Temp_actual();
			Setpoint = maxTemp - (grad*t);   
			myPID.Compute();
			Prev_state = RAMP_DOWN;    
			state=RELAY;

		break;

		case RELAY:
		  
			if(Prev_state == RAMP_DOWN || Prev_state == SOAK) Heat_on(Start_time);
			else Cool_on(Start_time); 

		break;

	}
 
}



void Heat_on(unsigned long Start_time){

	if(Output > millis() - Start_time)
	{
		// Abre las valvulas de vapor, purga y trampa por un tiempo proporcional al Output
        digitalWrite(FV202,HIGH);
        digitalWrite(FV208,HIGH);
        digitalWrite(FV209,HIGH);
	} 
	else if(millis() - Start_time < WindowSize)
	{
		// Cierra las valvulas durante el tiempo restante
        digitalWrite(FV202,LOW);
        digitalWrite(FV208,LOW);
        digitalWrite(FV209,LOW);
	}
	if(millis() - Start_time >= WindowSize)
	{ 
		Start_time += WindowSize;
		state = Prev_state;
	}  

}

void Cool_on(unsigned long Start_time){

	if(Output > millis() - Start_time)
	{
		// Abre las valvulas de Agua y retorno de enfriamiento
        digitalWrite(FV201,HIGH);
    	digitalWrite(FV203,HIGH);
	} 
	else if(millis() - Start_time < WindowSize)
	{
		// Cierra las valvulas
    	digitalWrite(FV201,LOW);
        digitalWrite(FV203,LOW);
	}
	if(millis() - Start_time >= WindowSize)
	{ 
		Start_time += WindowSize;
		state = RAMP_DOWN;
	} 
}


void Reset_process(){

	start = true;
	Step_Ok = false;
	State_RampU = 1;
	State_Soak = 1;	
	State_RampD = 1;

}

float Temp_actual(){

    float Temp = map(analogRead(TC100),129,664,-1778,14889);
    return Temp/100;
}

void End_Soak(){
	end_soak = true;
}

void Serial_msj(){

	double Tiempo_obtenido;
	Tiempo_obtenido = (float(millis()-init_Time2)/60000);
	Serial.println(" ========================================== ");
	Serial.println("Tiempo deseado: "+String(Tiempo_deseado));
	Serial.println("Tiempo obtenido: "+String(Tiempo_obtenido));
	Serial.println("Error: "+String(abs((Tiempo_obtenido - Tiempo_deseado))));
	Serial.println("Grad obtenido: "+String(float(maxTemp - init_Temp2)/Tiempo_obtenido));
	Serial.println(" ========================================== ");
}

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
