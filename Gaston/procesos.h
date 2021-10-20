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
int Calentamiento(int temp, float grad);
int Enfriamiento(int temp, float grad);
int Presurizado();
int Despresurizado();
int Temp_actual();

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
void Reset();


//Recetas.cpp
int Poliester(int temperatura, int tiempo);
int Algodon(int temperatura, int tiempo);
int Suavizado();
int Preblanqueo_quimico();
int Preblanqueo_jabon();
int Saponizado();

// Gaston.ino
void Interrupt();
void bNextCallback(void*ptr);
void bSelecCallback(void*ptr);
void bRightCallback(void*ptr);
void bLeftCallback(void*ptr);
void bCambiarEstadoCallback(void*ptr);
void SuavizadoSiCallback(void*ptr);
void SuavizadoNoCallback(void*ptr);
void send_msj(char msj[],int dato);
void send_Strmsj(char msj[]);
void Nextion_display(int Temp, int TempA, int Grad, int Tiempo, int Paso, int Aper, int Cierre);
void Act_tiempo(int tiempo);

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
void Lista_Poliester(int estado, int temperatura, int tiempo);
void Lista_Algodon(int estado, int temperatura, int tiempo);
void Lista_preblanqueo_quimico(int estado);
void Lista_preblanqueo_jabon(int estado);
void Lista_Saponizado(int estado);
void prueba(int estado);

#endif
