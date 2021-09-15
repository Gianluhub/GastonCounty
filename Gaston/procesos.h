#ifndef procesos_h
#define procesos_h




// prototipos

void Llenado(int Nivel);
void Llamado_op();
void Adicion_rapida(int tiempo);
void Adicion_lenta(int tiempo, int t_abierto, int t_cerrado);
int Circulacion(unsigned long interval);
unsigned long To_millis(int tiempo);
int timer1(unsigned long interval);
int timer2(unsigned long interval);


#endif
