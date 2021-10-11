#include <Arduino.h>
#include "Pines.h"
#include "procesos.h"

/* 
Llenado de tanque
Llena el tanque dependiendo del nivel deseado 
nivel 1 es el nivel bajo y nivel 2 el alto 
*/
// FALTA AVERIGUAR SOBRE EL ENCENDIDO DEL PLEGADOR
int Llenado(int Nivel){

    // Abre valvula para llenar el tanque y asegura que la de reflujo este abierta
    digitalWrite(FV200,HIGH);
    digitalWrite(FV204,HIGH);

    // Comunicacion por serial
    Serial.println("Llenado a nivel " + String(Nivel));
    Nextion_display(42,0,0,0,0,0,Nivel);

    // Cuando llegue al nivel deseado, cierra la valvula, enciendo el motor y el plegador
    switch(Nivel)
    {   
        // Llenado a nivel 1
        case 1:
            if (digitalRead(LC100) >= HIGH)
            {
               digitalWrite(FV200,LOW);
               digitalWrite(pump,HIGH); 
               digitalWrite(plegador_1,HIGH);
               //digitalWrite(plegador_2,HIGH);
               return true;
            }
            else return false;
        break;

        // Llenado a nivel 2
        case 2:
            if (digitalRead(LC101) >= HIGH)
            {
                digitalWrite(FV200,LOW);
                digitalWrite(pump,HIGH);
                digitalWrite(plegador_1,HIGH);
                //digitalWrite(plegador_2,HIGH);
                return true;
            }
            else return false;
        break;
    }

}
    

/*
Llamado de operador
Alarma que indica al operador que debe estar presente
No avanza al siguiente paso hasta que atienda el llamado
*/
int Llamado_op(){

    // Comunicacion por serial
    Serial.println("Llamado_op");
    Nextion_display(34,0,0,0,0,0,0);

    // Enciende la alarma para avisar al operador
    // Y espera a que este responda
    digitalWrite(LLAMADO_OP,HIGH);
    if (digitalRead(Op_ok) >= HIGH)
    {
        digitalWrite(LLAMADO_OP,LOW);
        return true;
    }
    else return false;

}



/*
Adicion rapida
*/

// La variable tiempo estara dada en minutos, hay que convertirla en millisegundos
int Adicion_rapida(int tiempo){

    // Comunicacion por serial
    Serial.println("Adicion_rapida " + String(tiempo) + " min");
   
    // Comunicacion con Nextion
    // El flag start se usa para mostrar en pantalla solo una vez por proceso
    // Se hace de esta forma porque al parecer se congela la pantalla si se le envian datos sin ningun delay de por medio
    static int start = true; 
    if (start)
    { 
        Nextion_display(89,0,0,0,tiempo,0,0);
        start = false; 
    }
    Act_tiempo(tiempo);


    // Pasa el tiempo de minutos a milisegundos 
    unsigned long tiempo_ms = To_millis(tiempo);
    Serial.println(tiempo_ms); 
    
    // Espera a que acabe el proceso
    if (timer1(tiempo_ms))
    {   
        Act_tiempo(false);  // Reiniciamos el contador
        start = true;    // Reiniciamos el flag para que la siguiente llamada de la funcion muestre en pantalla
        return true;
    }
    else return false;
}


/*
Adicion lenta
El parametro tiempo debe ser en minutos y los de t_abierto y t_cerrado en segundos
*/
int Adicion_lenta(int tiempo, int t_abierto, int t_cerrado ){


    // Comunicacion por serial
    Serial.println("Adicion_lenta " + String(tiempo)+" min");

    // Comunicacion con Nextion
    // El flag start se usa para mostrar en pantalla solo una vez por proceso
    // Se hace de esta forma porque al parecer se congela la pantalla si se le envian datos sin ningun delay de por medio
    static int start = true; 
    if (start)
    { 
        Nextion_display(88,0,0,0,tiempo,t_abierto,t_cerrado);
        start = false; 
    }
    Act_tiempo(tiempo);

    // Pasa el tiempo a milisegundos
    unsigned long tiempo_ms = To_millis(tiempo);
    unsigned long aux = t_abierto;
    unsigned long aux2 = t_cerrado;
    unsigned long t_ams = aux*1000;
    unsigned long t_cms = aux2*1000;

    // Mientras que no haya pasado el tiempo se seguiran abriendo la valvulas     
    if (!timer1(tiempo_ms))
    {
        /* Abre valvula de adicion cada cierto tiempo */
        digitalWrite(FV206,HIGH);
        delay(t_ams);
        digitalWrite(FV206,LOW);
        delay(t_cms);
        return false;
    }
    else 
    {
        Act_tiempo(false);  //Reiniciamos el contador
        start = true;
        return true;
    }
}


/*
 Circulacion
 Es la misma funcion timer pero debe recibir el tiempo en minutos.
*/

int Circulacion(int interval){

    // Comunicacion por serial
    Serial.println("Circulacion "+ String(interval));

    // Comunicacion con Nextion
    // El flag se usa para mostrar en pantalla solo una vez por proceso
    // Se hace de esta forma porque al parecer se congela la pantalla si se le envian datos sin ningun delay de por medio
    static int flag = true;
    if (flag)
    { 
        Nextion_display(1,0,0,0,interval,0,0);
        flag = false; 
    }
    Act_tiempo(interval);


    // Se pasan los tiempos a milisegundos
    unsigned long interval_ms = To_millis(interval);
    unsigned long currentTime = millis();
    static unsigned long previousTime = millis();
    static int start = 0;

    // Si start es 1, quiere decir que se comienza a contar de nuevo
    if (start == 1)
    { 
        previousTime = millis();
        start = 0;
    }

    // Calcula el tiempo faltante
    if (currentTime - previousTime >= interval_ms)
    {
        start = 1;
        flag = true;        // Reiniciamos flag para mostrar en pantalla
        Act_tiempo(false);  //Reiniciamos el contador
        return true;
    }
    else return false;

}


/*
 Lavado por rebose
 El parametro de tiempo esta en segundos.
 Mientras que corre el temporizador se estara realizando lecturas de 
 los sensores de nivel, cuando esta a nivel 2, abre la valvula de vaciado 
 por rebose FV210. Cuando el nivel 1 esta en LOW abre la valvula de agua 
 y cierra la de vaciado.
*/


int Lavado_rebose(int tiempo){


    // Comunicacion por serial
    Serial.println("Lavado rebose "+ String(tiempo) + " min");

    // Comunicacion con Nextion
    // El flag se usa para mostrar en pantalla solo una vez por proceso
    // Se hace de esta forma porque al parecer se congela la pantalla si se le envian datos sin ningun delay de por medio
    static int start = true;
    if (start)
    { 
        Nextion_display(50,0,0,0,tiempo,0,0);
        start = false; 
    }
    Act_tiempo(tiempo);


 
    // Pasamos el tiempo de minutos a milisegundos
    unsigned long tiempo_ms = To_millis(tiempo);

    // Se abre valvula de lavado por revose FV210 si es que no esta abierta la de agua
    if(!digitalRead(FV200)) digitalWrite(FV210,HIGH);

    /*
     Mientras que el temporizador esta contando, se realiza el lavado por rebose
    */
    if (!timer1(tiempo_ms))
    {   
        // Si el sensor de nivel 2 esta en HIGH
        // Abre la valvula de vaciado y cierra la de llenado
        if (digitalRead(LC101) >= HIGH)
        {
            digitalWrite(FV210,HIGH);
            digitalWrite(FV200,LOW);
        }
        // Si el sensor de nivel 1 esta en LOW
        // Abre la valvula de llenado y cierra la de vaciado
        if (digitalRead(LC100) <= LOW)
        {
            digitalWrite(FV210,LOW);
            digitalWrite(FV200,HIGH);
        }
    return false;
    }
    else
    {
    // Al finalizar el tiempo se asegura que las valvulas esten cerradas
    digitalWrite(FV210,LOW);
    digitalWrite(FV200,LOW);
    start = true;
    return true;
    }
}



/*
 Vaciado 
 Se hace por tiempo, hay que estimar el tiempo en que tarda en vaciar el tanque
 no requiere ningun parametro de entrada.
*/
int Vaciado(){

    Serial.println("Vaciado");
    Nextion_display(62,0,0,0,0,0,0);

    const unsigned long tiempo_vaciado = 10000;  // Falta poner el tiempo

    // Se abre la valvula de vaciado y se apaga la bomba y el plegador
    digitalWrite(pump,LOW);
    digitalWrite(plegador_1,LOW);
    //digitalWrite(plegador_2,LOW);
    digitalWrite(FV211,HIGH);

    // Espera el tiempo necesario donde se sabe que el tanque esta vacio
    // y luego cierra la valvula
    if(timer1(tiempo_vaciado))
    {
    Serial.println("Cierra vaciado");
    digitalWrite(FV211,LOW);
    return true;
    }
    else return false;
}




/*
    Calentamiento de tanque
    Recibe como entrada temperatura, gradiente y tiempo (aun no implementado)
    Si el gradiente es 0 significa que se subira la temperatura de forma directa
    es decir con las valvulas siempre abiertas.
    Retorna true cada vez que las valvulas esten cerradas para evitar problemas
    a la hora de cambiar de estado
*/

// NO ESTA CLARO AUN SI LAS VALVULAS DE TRAMPA Y PURGA DEBEN ESTAR CERRADAS DURANTE PRESURIZACION

int Calentamiento(int temp, float grad){

    // Comunicacion serial
    Serial.println("Calentamiento");
    Nextion_display(1,temp,Temp_actual(),grad,0,0,0);

    // Variables de utilidad
    unsigned long t_abierto = 2000;
    unsigned long t_cerrado = 3000;
    static int flag = true;


   //FALTA IMPLEMENTAR EL MAPEO DE TEMPERATURA

    if (Temp_actual() < temp)
    {   
        // Si no se requiere gradiente deja las valvulas abiertas siempre
        // No esta claro si en este caso tambien hay que dejar SIEMPRE abiertas
        // las valvulas de purga y de la trampa
        if (grad == 0)
        {
            digitalWrite(FV202,HIGH);
            digitalWrite(FV208,HIGH);
            digitalWrite(FV209,HIGH);
        }
        else
        {
            // AQUI FALTA IMPLEMENTAR LOS TIEMPOS CON EL GRADIENTE
            if (flag)
            {
                if(!timer2(t_abierto))
                {
                    // Abre las valvulas de vapor, purga y trampa por un tiempo t_abierto
                    digitalWrite(FV202,HIGH);
                    digitalWrite(FV208,HIGH);
                    digitalWrite(FV209,HIGH);
                }
                else flag = false;

            }
            else if (!timer2(t_cerrado))
            {
                // Cierra las valvulas durante un tiempo t_cerrado
                digitalWrite(FV202,LOW);
                digitalWrite(FV208,LOW);
                digitalWrite(FV209,LOW);
                return true;    
            }
            else flag = true;
        }
    }
    else 
    {
        // Asegura que las valvulas estan cerradas
        digitalWrite(FV202,LOW);
        digitalWrite(FV208,LOW);
        digitalWrite(FV209,LOW);

        return true;
    }
  
    return false;
   
}



/*
    Enfriamiento del tanque
    Recibe como entrada temperatura, gradiente y tiempo (aun no implementado)
    Si el gradiente es 0 significa que se subira la temperatura de forma directa
    es decir con las valvulas siempre abiertas.
*/

// NO ESTA CLARO AUN SI LAS VALVULAS DE TRAMPA Y PURGA DEBEN ESTAR CERRADAS DURANTE PRESURIZACION

int Enfriamiento(int temp, float grad){

    // Comunicacion serial
    Serial.println("Enfriamiento");
    Nextion_display(1,temp,Temp_actual(),grad,0,0,0);

    // Variables de utilidad
    unsigned long t_abierto = 2000;
    unsigned long t_cerrado = 3000;
    static int flag = true;
    
    // Asegura que las valvulas de calentamiento estan cerradas
    if (digitalRead(FV202) >= HIGH)
    {
        digitalWrite(FV202,LOW);
        digitalWrite(FV208,LOW);
        digitalWrite(FV209,LOW);
        digitalWrite(FV212,LOW);
    }

    if (Temp_actual() > temp)
    {   
        // Si no se requiere gradiente deja las valvulas abiertas siempre
    
        if (grad == 0)
        {
            digitalWrite(FV201,HIGH);
            digitalWrite(FV203,HIGH);
        
        }
        else
        {
            // AQUI FALTA IMPLEMENTAR LOS TIEMPOS CON EL GRADIENTE
            if (flag)
            {   
                if(!timer3(t_abierto))
                {
                    // Abre las valvulas de Agua y retorno de enfriamiento por un tiempo t_abierto
                    digitalWrite(FV201,HIGH);
                    digitalWrite(FV203,HIGH);
                }
                else flag = false;
            }

            else if (!timer3(t_cerrado))
            {   
                // Cierra las valvulas
                digitalWrite(FV201,LOW);
                digitalWrite(FV203,LOW);
            }
            else flag = true;
        }
    }
    else
    {
    // Asegurar que las valvulas estan cerradas
    digitalWrite(FV201,LOW);
    digitalWrite(FV203,LOW);
    return true;
    }

    return false;

}






/*
 Presurizacion
*/
    // REVISAR ESTA PARTE
int Presurizado(){

    static int flag = true;
    const int presion = 150; // Presion maxima, aun no se sabe cuanto vale

    // Si la temperatura es mayor a 80 y no se tiene el maximo de presion empieza a presurizar
    if (Temp_actual() >= 80 && digitalRead(PCH100) <= LOW)
    {   
        Serial.println("Comienza presurizado");
        if(flag)
        {
            if(!timer5(1500))
            {
                // Comienza el presurizado
                digitalWrite(FV204,LOW); // Cierra valvula de reflujo (deberian ser todas)
                digitalWrite(FV212,HIGH); // Se abre valvula de presurizado (entrada de aire)
            }
            else flag = false;
        }
            //delay(1500); // tiempo que permanece abierta la valvula de aire, aun a determinar

        else if(!timer5(3000)) 
        {
        digitalWrite(FV212,LOW); // Mantiene la valvula de aire cerrada por un tiempo
        }
        else flag = true;
    

    }
    else
    {
        // Si se llega a la max presion se cierra la valvula de aire
        digitalWrite(FV212,LOW);
        return true;
    }
    return false;
}


int Despresurizado(){

    static int flag = true;
    // Si la temperatura es menor a 80 y se tiene baja presion
    // Comienza el despresurizado
    // NO SE ESTA CLARO DE COMO SE TOMAN LAS LECTURAS DE LOS SENSORES DE PRESION
    if (Temp_actual() <= 80 && digitalRead(PCL100) <= LOW)
    {   
        Serial.println("Comienza despresurizado");
        if(flag)
        {
            if(!timer6(5000)) digitalWrite(FV213,HIGH); // Abre valvula de despresurizado durante 5 segundos
            else flag = false;
        }
        else if(!timer6(5000)) 
        {
        digitalWrite(FV213,LOW); // Cierra valvula de despresurizado por 5 segundos
        
        }
        else flag = true;
    }
    if (digitalRead(PCL100) >= HIGH)
    {
        digitalWrite(FV213,LOW);  // Cierra la valvula de despresurizado
        digitalWrite(FV204,HIGH); // Se abre la valvula de reflujo
        return true;
    }
    return false;
  
}

// Toma lectura del sensor de temperatura y lo traduce a 
// El valor maximo recibido por el transmisor es de 10V que por serial es 350
int Temp_actual(){

    int Temp_actual;
    Temp_actual = map(analogRead(TC100),0,350,0,400);
    return Temp_actual;
}




/*
    Funcion de suavizado

    Esta funcion es opcional en el proceso del teñido
    el operador debe tener la opcion de decidir si se desea realizar el suavizado de la tela
    esto se decide casi finalizando el programa.

    Se debe implementar una funcion que se comunique con el hmi para preguntar si se desea o no
    este proceso
*/

int Suavizado(){
    static int estado = 0;

    switch(estado)
    {   
        // 0. Realiza el llenado a nivel 1 si hace falta
        case 0:
            if(Llenado(1)) estado = 1;
        break;
        // 1. Llamado a operador para preparacion de tanque
        case 1:
            if(Llamado_op()) estado = 2;
        break;

        // 2. Adicion rapida de 2 min
        case 2:
            if(Adicion_rapida(2)) estado = 3;
        break;

        // 3. Circulacion de 20 min
        case 3:
            if(Circulacion(20)) estado = 4;
        break;

        // 4. Fin
        case 4:
            estado = 0;
            return true;
        break;
    }

    return false;

}
