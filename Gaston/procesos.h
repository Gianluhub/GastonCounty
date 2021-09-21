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
void Calentamiento(int temp, float grad);
void Enfriamiento(int temp, float grad);
void Presurizado();
void Despresurizado();
int Temp_actual();

// timer.cpp
unsigned long To_millis(int tiempo);
int timer1(unsigned long interval);
int timer2(unsigned long interval);
int timer3(unsigned long interval);
int timer4(unsigned long interval);
int timer5(unsigned long interval);

//Recetas.cpp
int Poliester(int temperatura);
int Algodon(int temperatura);
int Suavizado();
int Preblanqueo_quimico();
int Preblanqueo_jabon();
int Saponizado();


#endif
