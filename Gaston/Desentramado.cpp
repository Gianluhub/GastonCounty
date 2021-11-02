#include <Arduino.h>
#include "procesos.h"

extern char buffer[100];
//extern int temperatura[10];  
//extern int tiempo[10];



/*
  Guarda un dato en la posicion de un array de acuerdo al caracter que reciba.
  'b': Guarda los datos que pertenecen a los procesos de preblanqueo quimico, con jabon y saponizado.
  'p': Guarda los datos que pertenecen al poliester
  'a': Guarda los datos que pertenecen al algodon.
  'd': Guarda los datos que pertencen al directo.
  'e': Guarda el estado del proceso, usado en toma de muestra.
  'c': Guarda el codigo del proceso, usado en toma de muestra.
*/
void write_data(int array[], char save, int dato){

  switch(save) 
  {

      // Preblanqueos
      case 'b':
        array[0] = dato;
      break;

      // Poliester
      case 'p':
        array[1] = dato;
      break;

      // Algodon
      case 'a':
        array[2] = dato;
      break;

      // Algodon segundo dato de tiempo
      case 'f':
        array[3] = dato;
      break;
      // Directo
      case 'd':
        array[4] = dato;
      break;

      // Codigo
      case 'c':
        array[0] = dato;
      break;
      
      // Estado
      case 'e':
        array[1] = dato;
      break;
      
  }
}



/*
  Lee un dato en la posicion de un array de acuerdo al caracter que reciba.
  'b': Guarda los datos que pertenecen a los procesos de preblanqueo quimico, con jabon y saponizado.
  'p': Guarda los datos que pertenecen al poliester
  'a': Guarda los datos que pertenecen al algodon.
  'd': Guarda los datos que pertencen al directo.
  'e': Guarda el estado del proceso, usado en toma de muestra.
  'c': Guarda el codigo del proceso, usado en toma de muestra.
*/
int read_data(int array[], char dato){


  switch(dato) 
  {

      // Preblanqueos
      case 'b':
        return array[0];
      break;

      // Poliester
      case 'p':
        return array[1];
      break;

      // Algodon
      case 'a':
        return array[2];
        // No retorna el segundo valor del algodon usado para el tiempo
      break;

      // Algodon segundo dato de tiempo
      case 'f':
        return array[3] = dato;
      break;

      // Directo
      case 'd':
        return array[4];
      break;

      // Codigo
      case 'c':
        return array[0];
      break;

      // Estado
      case 'e':
        return array[1];
      break;
  }
}





/*
	Lee los datos del buffer y toma los datos de interes
	la trama recibida viene dada de en la forma: +A+D-130-60-+X
	Donde las letras representan un proceso como preblanqueo o poliester, los numeros representan
	unidades de temperatura y tiempo en ese mismo orden y la X se usa para indicar fin de la trama
*/
int desentramado(char trama[],int temperatura[], int tiempo[]){
  
  int i = 0;  // Indice del buffer
  int j = 0;  // Indice de trama
  int err = 0; // Contador de error
  int ERR = false; // se activa para salir del loop

  // Limpia los arrays
  memset(trama, 0, sizeof(trama)); 
  memset(temperatura, 0, sizeof(temperatura));
  memset(tiempo, 0, sizeof(tiempo));

  
 
  
  // Extrae los datos que representan los procesos a ejecutar hasta que se lea el caracter de fin 'X'
  do
  {
    // Revisa si hay un dato nuevo que tomar
    if(buffer[i] == '+')
    {
      do
      {
        trama[j] = buffer[i+1];   // Guarda el dato de interes
        i+=2;                   // Incrementa para saltar al siguiente dato a extraer
        j++;                    // Incrementa la posicion del array  
      }while(buffer[i]!='+');
      i++;

    }else
    {  
      /*
        Extrae los valores de tiempo y temperatura del buffer
        Los datos que tienen el caracter de inicio '-' son de temperatura
        Los datos que tienen el caracter de inicio '#' son de tiempo
        
        Los caracteres 'p','a','b', 'd' hacen referencia a que el dato pertenece
        a un proceso de poliester, algodon, preblanqueo o directo, respectivamente.

        La temperatura y el tiempo se guardados en sus respectivos arrays, donde la posicion del array 
        donde es almacenada la data depende de a quien pertenezca el dato.

        Ejemplo: Si se tiene un dato del tipo -b98- este hace referencia a un dato de temperatura para el proceso
        de preblanqueo, por lo que el valor estara guardado en la posicion 3 del array temperatura (temperatura[2])
        
      */
      i = Tomar_Dato(i,'-',buffer,temperatura);
      i = Tomar_Dato(i,'#',buffer,tiempo);
    }
    
    if(err >= sizeof(buffer))
    {
      err = 0;
      Serial.println("Error");
      return false;
    }
    err++;

  }while(buffer[i]!='X' || ERR);
  trama[j] = 'X';
  
  return true;
}


/*
  Extrae los datos de interes al detectar un caracter de inicio y guarda
  los datos en formato entero.
  Retorna el indice que apunta a la siguiente posicion de la trama
  Se utiliza principalmente para guardar valores de tiempo o temperatura. 
*/
int Tomar_Dato(int i,char start, char buffer[], int save[]){

  int k = 0;  // Indice de aux
  char aux[4] = {0};  // Array auxiliar que almacena los datos de interes
  char aux2 = 0;      // Array auxiliar que almacena el caracter 
 

  // Verifica si es necesario extraer un valor 
  if (buffer[i] == start)
  {
    aux2 = buffer[i+1];     // Guarda el caracter que indica a qué proceso pertenece
    i+=2;                   // Incrementa al siguiente dato a extraer

    // Extrae los valores de temperatura hasta que se lea el caracter de fin
    do
    {
      aux[k] = buffer[i];
      k++;
      i++;
    }while(buffer[i]!=start);

    i++;   // Aumenta el indice de la trama para la siguiente iteracion
    
    // Guarda el dato extraido
    write_data(save,aux2,String(aux).toInt());

    // if(aux2 == save1) save[0] = String(aux).toInt();  // Convierte los caracteres a un entero y se guarda    
    // else if(aux2 == save2) save[1] = String(aux).toInt();  // Convierte los caracteres a un entero y se guarda
    // else if(aux2 == save3) save[2] = String(aux).toInt();  // Convierte los caracteres a un entero y se guarda 
    // else if(aux2 == save4) save[3] = String(aux).toInt();  // Convierte los caracteres a un entero y se guarda 
  }

  return i;   // retorna el siguiente dato a leer

}
