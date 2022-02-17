#ifndef procesos_h
#define procesos_h




// Prototipos

// Procesos.cpp
int Llenado(int Nivel);
int Llamado_op();
int Adicion_rapida(int tiempo);
int Adicion_lenta(int tiempo, int t_abierto, int t_cerrado);
int Circulacion(int interval);
int Lavado_rebose(int tiempo);
int Vaciado();
int Calentamiento(int temp, float grad,int mode);
int Enfriamiento(int temp, float grad);
int Presurizado();
int Despresurizado();
void Cerrar_presurizado();
int Handler_motores(int state);

// timer.cpp
unsigned long To_millis(int tiempo);
int timer1(unsigned long interval);
int timer2(unsigned long interval);
int timer3(unsigned long interval);
int timer4(unsigned long interval);
int timer5(unsigned long interval);
int timer6(unsigned long interval);
int timer7(unsigned long interval);
int timer8(unsigned long interval);
int timer9(unsigned long interval);
int timer10(unsigned long interval);
int timer11(unsigned long interval);
int timer12(unsigned long interval);
void Reset();


//Recetas.cpp
int Poliester(int temperatura, int tiempo);
int Algodon(int temperatura, int tiempoC,int tiempoF);
int Suavizado();
int Preblanqueo_quimico(int temperatura, int tiempo);
int Preblanqueo_jabon(int temperatura, int tiempo);
int Saponizado(int temperatura, int tiempo);
int Directo(int temperatura, int tiempo);
int Lavado_Maquina();
int Lavado_reductivo(int temperatura, int tiempo);
int Prueba(int temperatura, int tiempo);


// Gaston.ino
void Interrupt();
void bNextCallback(void*ptr);
void bSelecCallback(void*ptr);
void bRightCallback(void*ptr);
void bLeftCallback(void*ptr);
void bCambiarEstadoCallback(void*ptr);
void bBackCallback(void*ptr);
void SuavizadoSiCallback(void*ptr);
void SuavizadoNoCallback(void*ptr);
void LavadoRedSiCallback(void*ptr);
void send_msj(char msj[],int dato);
void send_Strmsj(char msj[]);
void Nextion_display(int Temp, int TempA, int Grad, int Tiempo, int Paso, int Aper, int Cierre);
void Seleccion_proceso(int codigo, int estado);
void Act_tiempo(int tiempo);


// Desentramado.cpp
int desentramado(char trama[],int temperatura[], int tiempo[]);
int Tomar_Dato(int i,char start, char buffer[], int save[]);
void write_data(int array[], char save, int dato);
int read_data(int array[], char dato);



// procesos_print.cpp
void Llenado_print(int Nivel);
void Llamado_op_print();
void Adicion_rapida_print(int tiempo);
void Adicion_lenta_print(int tiempo, int t_abierto, int t_cerrado);
void Circulacion_print(int interval);
void Lavado_rebose_print(int tiempo);
void Vaciado_print();
void Calentamiento_print(int temp, float grad);
void Enfriamiento_print(int temp, float grad);
void Preguntar_print();
void Suavizado_print();
void Tomar_muestra_print();
void Fin_proceso();

// tomar_muestra.cpp
int Tomar_muestra(int estado);
int Preguntar_Suavizado();
int Preguntar_Lavado();
void Lista_Poliester(int estado, int temperatura, int tiempo);
void Lista_Algodon(int estado, int temperatura, int tiempoC, int tiempoF);
void Lista_preblanqueo_quimico(int estado, int temperatura, int tiempo);
void Lista_preblanqueo_jabon(int estado, int temperatura, int tiempo);
void Lista_Saponizado(int estado,int temperatura, int tiempo);
void Lista_Directo(int estado, int temperatura, int tiempo);
void Lista_Lavado_reductivo(int estado, int temperatura, int tiempo);
void Lista_Lavado_Maquina(int estado);
void Lista_Suavizado(int estado);
void prueba(int estado, int temperatura, int tiempo);

// Valvulas.cpp
void Detener_proceso();
void Reiniciar_proceso();
void Callback_ISR();

// PID.cpp
#define RAMP_UP 1
#define SOAK 2
#define RAMP_DOWN 3
#define RELAY 4
#define RELAY2 5
// Funciones
float Temp_actual();
void PID_ON();
int timerPID(unsigned long interval);
void Setup_RAMP_UP();
void Setup_RAMP_DOWN();
void Setup_SOAK();
void HeatGrad_1_5();
int State_PID(int Mode, int Temp);
void Reset_PID();
void End_Soak();

// Pruebas.cpp
void Test();
void Handler();
int Statechange(int pin, int state, char msj[],bool cont);
int Next();
int Next2();
void bActCallback(void*ptr);
void contador_llenado();
void contador_Adicion_lenta();
void contador_rebose();
void contador_vaciado();
void contador_Calentamiento();
void contador_Enfriamiento();
void reiniciar_contadores();
void Incrementar_contador(int pin);

#endif
