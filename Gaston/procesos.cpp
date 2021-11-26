#include <Arduino.h>
#include "Pines.h"
#include "procesos.h"


int Mostrar = true;     // Muestra en pantalla una sola vez por proceso, esto se hace para no causar problemas de comunicacion por puerto serial

/* 
Llenado de tanque
Llena el tanque dependiendo del nivel deseado 
nivel 1 es el nivel bajo y nivel 2 el alto 
*/
// FALTA AVERIGUAR SOBRE EL ENCENDIDO DEL PLEGADOR
int Llenado(int Nivel){

    // Muestra en pantalla el proceso, se hace solo una vez para no saturar el puerto serial.
    if(Mostrar)
    { 
        Llenado_print(Nivel);
        Mostrar = false;     
    } 
    
    // Asegura que la valvula de reflujo esta abierta
    digitalWrite(FV204,HIGH);

    // Cuando llegue al nivel deseado, cierra la valvula, enciende el motor y el plegador
    switch(Nivel)
    {   
        // Llenado a nivel 1
        case 1:
            if (digitalRead(LC100) >= HIGH)
            {
               // Cierra valvula de agua y enciende la bomba y el plegador
               digitalWrite(FV200,LOW);
               digitalWrite(pump,HIGH); 
               digitalWrite(plegador_1,HIGH);
               digitalWrite(jet_1,HIGH);
               //digitalWrite(plegador_2,HIGH);
               contador_llenado();
               Mostrar = true;
               return true;
            }
            else{
                // Si no esta activo el sensor de nivel, abre la valvula de agua
                digitalWrite(FV200,HIGH);
                return false;
                }
        break;

        // Llenado a nivel 2
        case 2:
            if (digitalRead(LC101) >= HIGH)
            {   
                // Cierra valvula de agua y enciende la bomba y el plegador
                digitalWrite(FV200,LOW);
                digitalWrite(pump,HIGH);
                digitalWrite(plegador_1,HIGH);
                digitalWrite(jet_1,HIGH);
                //digitalWrite(plegador_2,HIGH);
                Mostrar = true;
                contador_llenado();
                return true;
            }
            else
            {   
                // Si no esta activo el sensor de nivel, abre la valvula de agua
                digitalWrite(FV200,HIGH);
                return false;
            }
        break;
    }

}
    

/*
Llamado de operador
Alarma que indica al operador que debe estar presente
No avanza al siguiente paso hasta que atienda el llamado
*/
int Llamado_op(){

    
    Handler_motores(true);
    // Muestra en pantalla el proceso, se hace solo una vez para no saturar el puerto serial.
    if(Mostrar)
    { 
        Llamado_op_print();
        Mostrar = false;
        
    } 

    // Asegura que la valvula de reflujo esta abierta
    digitalWrite(FV204,HIGH);

    // Enciende la alarma para avisar al operador
    // Y espera a que este responda
    digitalWrite(LLAMADO_OP,HIGH);
    if (digitalRead(Op_ok) >= HIGH)
    {
        digitalWrite(LLAMADO_OP,LOW);
        Mostrar = true;
        return true;
    }
    else return false;

}



/*
Adicion rapida
*/

// La variable tiempo estara dada en minutos, hay que convertirla en millisegundos
int Adicion_rapida(int tiempo){

    Handler_motores(true);

    // Comunicacion con Nextion
    // El flag Mostrar se usa para mostrar en pantalla solo una vez por proceso
    // Se hace de esta forma porque al parecer se congela la pantalla si se le envian datos sin ningun delay de por medio

    if (Mostrar)
    { 
        Adicion_rapida_print(tiempo);
        Mostrar = false; 
    }
    Act_tiempo(tiempo);

    // Asegura que la valvula de reflujo esta abierta
    digitalWrite(FV204,HIGH);

    // Pasa el tiempo de minutos a milisegundos 
    unsigned long tiempo_ms = To_millis(tiempo);
     
    
    // Espera a que acabe el proceso
    if (timer1(tiempo_ms))
    {   
        Act_tiempo(false);  // Reiniciamos el contador
        Mostrar = true;    // Reiniciamos el flag para que la siguiente llamada de la funcion muestre en pantalla
        Serial.println("Adicion rapida true");
        return true;
    }
    else return false;
}


/*
Adicion lenta
El parametro tiempo debe ser en minutos y los de t_abierto y t_cerrado en segundos
*/
int Adicion_lenta(int tiempo, int t_cerrado, int t_abierto ){

    Handler_motores(true);
    // Comunicacion con Nextion
    // El flag Mostrar se usa para mostrar en pantalla solo una vez por proceso
    // Se hace de esta forma porque al parecer se congela la pantalla si se le envian datos sin ningun delay de por medio
    if (Mostrar)
    { 
        Adicion_lenta_print(tiempo,t_cerrado,t_abierto);
        Mostrar = false; 
    }
    Act_tiempo(tiempo);

    // Asegura que la valvula de reflujo esta abierta
    digitalWrite(FV204,HIGH);

    // Pasa el tiempo a milisegundos
    unsigned long tiempo_ms = To_millis(tiempo);
    unsigned long aux = t_abierto;
    unsigned long aux2 = t_cerrado;
    unsigned long t_ams = aux*1000;
    unsigned long t_cms = aux2*1000;

    // Flag para control de valvulas
    static int flag = true;

   // Mientras que no haya pasado el tiempo se seguiran abriendo la valvulas  
    if(!timer1(tiempo_ms))
    {
         if (flag)
        {
            if(!timer8(t_ams))
            {
                // Abre la valvula de dosificacion de aditivos
               digitalWrite(FV206,HIGH);
               Serial.println("Abierto");
            }
            else flag = false;

        }
        else if (!timer8(t_cms))
        {
            // Cierra las valvulas durante un tiempo t_cerrado
            digitalWrite(FV206,LOW);
            Serial.println("Cerrado");
                 
        }
        else flag = true;
    }
    else 
    {
        digitalWrite(FV206,LOW); // Cerramos la valvulas
        Act_tiempo(false);  //Reiniciamos el contador
        flag = true;
        Mostrar = true;
        timer8(false);
        Serial.println("Adicion lenta true");
        contador_Adicion_lenta();
        return true;
    }
    return false;
}

/*
 Circulacion
 Es la misma funcion timer pero debe recibir el tiempo en minutos.
*/

int Circulacion(int interval){

    Handler_motores(true);
    // Comunicacion con Nextion
    // El flag se usa para mostrar en pantalla solo una vez por proceso
    // Se hace de esta forma porque al parecer se congela la pantalla si se le envian datos sin ningun delay de por medio
    if (Mostrar)
    { 
        Circulacion_print(interval);
        Mostrar = false; 
    }
    Act_tiempo(interval);

    // Se pasan los tiempos a milisegundos
    unsigned long interval_ms = To_millis(interval);

    // Calcula el tiempo faltante
    if (timer9(interval_ms))
    {
        Mostrar = true;        // Reiniciamos flag para mostrar en pantalla
        Act_tiempo(false);  //Reiniciamos el contador
        Serial.println("Circulacion true");
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

    
    Handler_motores(true);

    // Comunicacion con Nextion
    // El flag se usa para mostrar en pantalla solo una vez por proceso
    // Se hace de esta forma porque al parecer se congela la pantalla si se le envian datos sin ningun delay de por medio
    if (Mostrar)
    { 
        Lavado_rebose_print(tiempo);
        Mostrar = false; 
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
        Act_tiempo(false);       // Reiniciamos contador
        Mostrar = true;
        Serial.println("Rebose true");
        contador_rebose();
        return true;
    }
}



/*
 Vaciado 
 Se hace por tiempo, hay que estimar el tiempo en que tarda en vaciar el tanque
 no requiere ningun parametro de entrada.
*/
int Vaciado(){

    // Comunicacion con Nextion
    // El flag se usa para mostrar en pantalla solo una vez por proceso
    // Se hace de esta forma porque al parecer se congela la pantalla si se le envian datos sin ningun delay de por medio
    if (Mostrar)
    { 
        Vaciado_print();
        Mostrar = false; 
    }

    const unsigned long tiempo_vaciado = 300000;  // Falta poner el tiempo

    // Se abre la valvula de vaciado y se apaga la bomba y el plegador
    digitalWrite(pump,LOW);
    digitalWrite(plegador_1,LOW);
    digitalWrite(jet_1,LOW);
    //digitalWrite(plegador_2,LOW);
    digitalWrite(FV211,HIGH);

    if(digitalRead(LC100) <= LOW)
    {
        // Espera el tiempo necesario donde se sabe que el tanque esta vacio
        // y luego cierra la valvula
        if(timer1(tiempo_vaciado))
        {
            Serial.println("Cierra vaciado");
            digitalWrite(FV211,LOW);
            Mostrar = true;
            contador_vaciado();
            return true;

        }else return false;
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

int Calentamiento(int temp, float grad,int mode){

    Handler_motores(true);

    // Comunicacion con Nextion
    // El flag se usa para mostrar en pantalla solo una vez por proceso
    // Se hace de esta forma porque al parecer se congela la pantalla si se le envian datos sin ningun delay de por medio
    if (Mostrar)
    { 
        Calentamiento_print(temp,grad);
        Mostrar = false; 
    }
    // Actualiza la temperatura cada 100ms
    if(timer7(100)) send_msj("nTempA.val=",Temp_actual());

    // Variables de utilidad
    //unsigned long t_abierto = 2000;
    //unsigned long t_cerrado = 3000;
    //static int flag = true;
    int pid_ok;

    if(temp > 100) Presurizado();


    pid_ok = State_PID(mode,temp);

    if(pid_ok)
    {   
        if(digitalRead(FV213) >= HIGH) digitalWrite(FV212,LOW);
        Mostrar = true;
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

    Handler_motores(true);

    // Comunicacion con Nextion
    // El flag se usa para mostrar en pantalla solo una vez por proceso
    // Se hace de esta forma porque al parecer se congela la pantalla si se le envian datos sin ningun delay de por medio
    if (Mostrar)
    { 
        Enfriamiento_print(temp,grad);
        Mostrar = false; 
    }
    // Actualiza la temperatura cada 100ms
    if(timer7(100)) send_msj("nTempA.val=",Temp_actual());

    // Variables de utilidad
    //unsigned long t_abierto = 2000;
    //unsigned long t_cerrado = 3000;
    //static int flag = true;
    int pid_ok;

    // Asegura que las valvulas de calentamiento estan cerradas
    if (digitalRead(FV202) >= HIGH)
    {
        digitalWrite(FV202,LOW);
        digitalWrite(FV208,LOW);
        digitalWrite(FV209,LOW);
        digitalWrite(FV212,LOW);
    }

    pid_ok = State_PID(RAMP_DOWN,60);

    if(pid_ok)
    {   
        if(digitalRead(FV213) >= HIGH)
        {
            digitalWrite(FV213,LOW);  // Cierra la valvula de despresurizado
            digitalWrite(FV204,HIGH); // Se abre la valvula de reflujo
        }
        Mostrar = true;
        return true;
    }

    return false;

}






/*
 Presurizacion
*/
    // REVISAR ESTA PARTE
int Presurizado(){

    //static int flag = true;  // Sirve de ayuda para implementar los delays con un mismo contador
   // const int presion = 150; // Presion maxima, aun no se sabe cuanto vale

    // Si la temperatura es mayor a 85 y no se tiene el maximo de presion empieza a presurizar
    if (Temp_actual() >= 85 && digitalRead(PCH100) <= LOW)
    {   
        Serial.println("Comienza presurizado");
        // if(flag)
        // {
        //     if(!timer5(1500))
        //     {
        //         // Comienza el presurizado
        //         Cerrar_presurizado(); // Cierra todas las valvulas salvo las que se encarguen de la temperatura y presurizacion
        //         digitalWrite(FV212,HIGH); // Se abre valvula de presurizado (entrada de aire)
        //     }
        //     else flag = false;
        // }
        //     //delay(1500); // tiempo que permanece abierta la valvula de aire, aun a determinar

        // else if(!timer5(3000)) 
        // {
        // digitalWrite(FV212,LOW); // Mantiene la valvula de aire cerrada por un tiempo
        // }
        // else flag = true;
    
        //Comienza el presurizado
        Cerrar_presurizado(); // Cierra todas las valvulas salvo las que se encarguen de la temperatura y presurizacion
        digitalWrite(FV212,HIGH); // Se abre valvula de presurizado (entrada de aire)
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
float Temp_actual_Pot(){

    float Temp_actual;
    Temp_actual = map(analogRead(TC100),0,350,0,400);
    return Temp_actual;
}



// Cierra todas las valvulas salvo las encargadas de suministrar vapor a la maquina y de presurizar
void Cerrar_presurizado(){

    digitalWrite(FV200,LOW);
    digitalWrite(FV201,LOW);
    digitalWrite(FV203,LOW);
    digitalWrite(FV204,LOW);
    digitalWrite(FV205,LOW);
    digitalWrite(FV206,LOW);
    digitalWrite(FV207,LOW);
    digitalWrite(FV210,LOW);
    digitalWrite(FV211,LOW);
    digitalWrite(FV213,LOW);
    digitalWrite(FV214,LOW);             
}

// Chequea que las valvulas y los motores esten en su estado que corresponda
// Esta funcion se usa para evitar que algunas valvulas o que los motores queden apagados
// despues de haber realizado una interupcion o cambio de estado por toma de muestra.
void Handler_motores(int state){

    

    if(state)
    {
            
        if(digitalRead(LC100) >=HIGH || digitalRead(LC101) >= HIGH || true){

            if(digitalRead(pump) <= LOW || digitalRead(plegador_1) <= LOW || digitalRead(jet_1) <= LOW)
            {      
                digitalWrite(pump,HIGH);
                digitalWrite(plegador_1,HIGH);
                digitalWrite(jet_1,HIGH);
            }
        }else
        {
            if(digitalRead(pump) >= HIGH || digitalRead(plegador_1) >= HIGH || digitalRead(jet_1) >= HIGH)
            {      
                digitalWrite(pump,LOW);
                digitalWrite(plegador_1,LOW);
                digitalWrite(jet_1,LOW);
            }
        }

    }else
    {
        digitalWrite(pump,LOW);
        digitalWrite(plegador_1,LOW);
        digitalWrite(jet_1,LOW);
    }


}
