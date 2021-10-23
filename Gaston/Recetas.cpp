#include <Arduino.h>
#include <Nextion.h>
#include "Pines.h"
#include "procesos.h"


// Funcion de seguridad
// Cierra las valvulas de calentamiento y presurizado
void cerrar_vapor(){

	digitalWrite(FV202,LOW);
	digitalWrite(FV208,LOW);
	digitalWrite(FV209,LOW);
	digitalWrite(FV212,LOW);
}


int Poliester(int temperatura, int tiempo){

	static int estado = 1;				// Variable encargada de pasar de un proceso a otro		
	int temp_ok = false;				// Verifica si se llego a la temperatura deseada
	int press_ok = false;				// Verifica si se llego a la presion deseada

	static int check_state = 0;			// Se uitiliza para actualizar en la pantalla el paso del proceso
	if (check_state != estado)
	{
		send_msj("nPaso.val=",estado);		// Muestra en pantalla el paso del proceso
		check_state = estado;
	}

	switch (estado)
	{
		// 1. Llenado a nivel 2
		case 1:
			if (Llenado(2)) estado = 2;	
		break;

		// 2. Preparaciond de tanque
		case 2:
			if(Llamado_op()) estado = 3;
		break;

		// 3. Adicion rapida 5 min
		case 3:
			if(Adicion_rapida(5)) estado = 4;
		break;


		// 4. Circulacion 10 min
		case 4:
			if (Circulacion(10)) estado = 5;
		break;


		// 5. Preparacion de tanque
		case 5:
			if(Llamado_op()) estado = 6;
		break;

		// 6. Adicion Lenta
		case 6:
			if(Adicion_lenta(20,20,2)) estado = 7;
		break;

		// 7. Circulacion 20 min
		case 7:
			if (Circulacion(20)) estado = 8;
		break;

		// 8. Subir temperatura
		case 8:
			// Si la temperatura es 130 primero suber a 90° a 2°/min
			// Luego sube a 130° a 1.5°/min
			if (temperatura == 130)
			{
				if (Temp_actual() < 90) temp_ok = Calentamiento(90,2);
				else if ( Temp_actual() >= 90) temp_ok = Calentamiento(130,1.5);	
			}
			else temp_ok = Calentamiento(temperatura,2);
			press_ok =  Presurizado();

			if ( Temp_actual() >= temperatura - 1 && temp_ok && press_ok) estado = 9;
			
		break;

		// 9. Circulacion por 1 hora y se asegura de mantener la temperatura deseada
		case 9:

			Calentamiento(temperatura,2);
			Presurizado();	
			if (Circulacion(tiempo))
			{
				cerrar_vapor();
				estado = 10;
			}	
		break;

		// 10. Enfriamiento
		case 10:
			temp_ok = Enfriamiento(60,2);
			press_ok = Despresurizado();
			if (Temp_actual() <= 60 && temp_ok && press_ok) estado = 11;	
		break;
		
		// 11. Toma de muestra
		case 11:
			estado = Tomar_muestra(estado);
		break;

		// 12. Lavado por rebose de 15 min
		case 12:
			if (Lavado_rebose(15)) estado = 13;
		break;

		// 13. Vaciado de tanque
		case 13:
			if (Vaciado()) estado = 14;
		break;

		// 14. Llenado a nivel 2
		case 14:
			if(Llenado(2)) estado = 15;
		break;

		// 15. Lavado por rebose de 15 min
		case 15:
			if(Lavado_rebose(15)) estado = 16;
		break;

		// 16. Vaciado
		case 16:
			if (Vaciado()) estado = 17;
		break;

		// // Parte nueva añadida (el suavizado debe ser opcional)
		// // 17. Preguntar por suavizado
		// case 17:
		// 	if(Preguntar_Suavizado()) estado = 18;
		// break;

		// 17. Toma de muestra
		case 17:
			estado = Tomar_muestra(estado);
		break;

		// 18. Fin
		case 18:
			Fin_proceso();
			estado = 1;
			return true;
		break;	
	}

	return false;
}


// REVISAR LO DE MANTENER TEMPERATURA A LO LARGO DEL PROCESO
// NO SE ESTA CLARO SI EN EL LAVADO POR REBOSE SE DEBE SEGUIR CALENTANDO
// PREGUNTAR POR PRESURIZADO A 90 NO ESTA CLARO SI NECESARIAMENTE DEBE LLEGAR  A LA MAX PRESION
// OJO LINEA 294
int Algodon(int temperatura, int tiempoC, int tiempoF){

	static int estado = 1;			// Variable encargada de pasar de un proceso a otro		
	int temp_ok = false;			// Verifica si se llego a la temperatura deseada
	int press_ok = false;			// Verifica si se llego a la presion deseada
	
	static int check_state = 0;			// Se uitiliza para actualizar en la pantalla el paso del proceso
	if (check_state != estado)
	{
		send_msj("nPaso.val=",estado);		// Muestra en pantalla el paso del proceso
		check_state = estado;
	}

	switch(estado){


		// 1. Llenado a nivel 1
		case 1:
			if (Llenado(1)) estado = 2;
		break;

		// 2. Llamado a operador
		case 2:
			if(Llamado_op()) estado = 3;
		break;

		// 3. Adicion rapida 30 min
		case 3:
			if(Adicion_rapida(30)) estado = 4;
		break;

		// 4. Circulacion 10 min
		case 4:
			if(Circulacion(10)) estado = 5;
		break;

		// 5. Llamado a operador
		case 5:
			if(Llamado_op()) estado = 6;
		break;

		// 6. Adicion lenta 25 min cada 20 seg por 2 seg 
		case 6:
			if(Adicion_lenta(25,20,2)) estado = 7;
		break;

		// 7. Subir temperatura (puede ser a 60 o 80) no presuriza
		case 7:
			temp_ok = Calentamiento(temperatura,1.5);
			if (Temp_actual() >= temperatura && temp_ok) estado = 8;
		break;

		// 8. Circulacion
		case 8:
			Calentamiento(temperatura,2);
			if(Circulacion(tiempoC)) estado = 9;		
		break;

		// 9. Llamado a operador y mantener temperatura
		case 9:
			Calentamiento(temperatura,2);
			if(Llamado_op()) 
			{
				cerrar_vapor();
				estado = 10;
			}	
		break;

		// 10. Adicion lenta (Alkali) de 5 min cada 10 seg por 2 seg
		case 10:
			if(Adicion_lenta(5,10,2)) estado = 11;
		break;

		// 11. Adicion lenta (Alkali) de 25 min cada 10 seg por 3 seg
		case 11:
			if(Adicion_lenta(25,10,3)) estado = 12;
		break;

		// 12. Adicion lenta (Alkali) 15 min cada 15 seg por 3 seg
		case 12:
			if(Adicion_lenta(15,15,3)) estado = 13;
		break;

		// 13. Circulacion por 1 hora y manterner temperatura
		case 13:
			Calentamiento(temperatura,1.5);
			if (Circulacion(tiempoF)) 
			{	
				cerrar_vapor();
				estado = 14;
			}
		break;

		// 14. Llamado a operador (Tomar muestra)
		case 14:
			estado = Tomar_muestra(estado);
		break;

		// 15. Lavado por rebose de 10 min
		case 15:
			if(Lavado_rebose(10)) estado = 16;
		break;

		// 16. Vaciado de tanque
		case 16:
			if(Vaciado()) estado = 17;
		break;

		//17. Llenado a nivel 2
		case 17:
			if(Llenado(2)) estado = 18;
		break;

		// 18. Llamado a operador
		case 18:
			if(Llamado_op()) estado = 19;
		break;

		// 19. Adicion rapida de 3 min. Acido
		case 19:
			if(Adicion_rapida(3)) estado = 20;
		break;


		// 20. Subir temperatura a 60°
		case 20:
			temp_ok = Calentamiento(60,2);
			if (Temp_actual() >= 60 && temp_ok) estado = 21;
		break;

		// 21. Circulacion por 10 min
		case 21:
			Calentamiento(60,2);
			if(Circulacion(10))
			{
				cerrar_vapor();
				estado = 22;
			}
		break;

		// 22. Vaciado de tanque
		case 22:
			if(Vaciado()) estado = 23;
		break;

		//23. Llenado a nivel 2
		case 23:
			if(Llenado(2)) estado = 24;
		break;

		// 24. Llamado a operador
		case 24:
			if(Llamado_op()) estado = 25;
		break;

		// 25. Adicion rapida de 3 min. Detergente
		case 25:
			if(Adicion_rapida(3)) estado = 26;
		break;

		// 26. Circulacion por 5 min
		case 26:
			if(Circulacion(5)) estado = 27;
		break;

		// Enjabonado en caliente
		// 27. Subir temperatura a 90° a 2°/min
		case 27:
			temp_ok = Calentamiento(90,2);
			Presurizado();
			if (Temp_actual() >= 90 && temp_ok) estado = 28;
		break;

		// 28. Circulacion por 20 min
		case 28:
			Calentamiento(90,2);
			Presurizado();
			if(Circulacion(20))
			{
				cerrar_vapor();
				estado = 29;
			}
		break;

		// 29. Enfriamiento a 60° a 2°/min
		case 29:
			temp_ok = Enfriamiento(60,2);
			press_ok = Despresurizado();
			if (Temp_actual() <= 60 && temp_ok && press_ok) estado = 30;
		break;

		// 30. Vaciado de tanque
		case 30:
			if(Vaciado()) estado = 31;
		break;

		// 31. Llenado a nivel 2
		case 31:
			if(Llenado(2)) estado = 32;
		break;

		// 32. Subir temperatura a 65°
		case 32:
			temp_ok = Calentamiento(65,2);
			if ( Temp_actual() >= 65 && temp_ok) estado = 33;
		break;

		// 33. Circulacion por 10 min
		case 33:
			Calentamiento(65,2);
			if(Circulacion(10))
			{
			cerrar_vapor();
			estado = 34;
			}
		break;

		// 34. Vaciado de tanque
		case 34:
			if(Vaciado()) estado = 35;
		break;

		//  35. Llenado a nivel 2
		case 35:
			if(Llenado(2)) estado = 36;
		break; 

		// 36. Tomar muestra en este caso se agrega una funcion adicional
		// Para que el operador salte el lavado opcional al precionar el boton de llamado de op
		// Si desea realizar el proceso debera escogerlo manualmente dentro de la pantalla.
		case 36:
			estado = Tomar_muestra(estado);
		break;

		/// PARTE OPCIONAL PARA EL OPERADOR 

		// 37. Circulacion por 10 min
		case 37:
			if(Circulacion(10)) estado = 38;
		break;

		// 38. Vaciado de tanque
		case 38:
			if(Vaciado()) estado = 39;
		break;

		// 39. Llenado a nivel 2
		case 39:
			if(Llenado(2)) estado = 40;
		break;

		// 40. Lavado por rebose de 15 min
		case 40:
			if(Lavado_rebose(15)) estado = 41;
		break; 

		// 41. Vaciado de tanque
		case 41:
			if(Vaciado()) estado = 42;
		break;

		// 42. Llenado a nivel 2
		case 42:
			if(Llenado(2)) estado = 43;
		break;

		/// FIN DE PROCESO OPCIONAL

		// 43. Llamado a operador
		case 43:
			if(Llamado_op()) estado = 44;
		break;	

		// 44. Adicion rapida. Fijador
		case 44:
			if(Adicion_rapida(5)) estado = 45;
		break;

		// 45. Circulacion por 20 min
		case 45:
			if(Circulacion(20)) estado = 46;
		break; 

		// Parte nueva añadida (el suavizado debe ser opcional)
		// 46. preguntar si se desea suvizado de tela (Suavizado es opcional)
		// case 46:
		// 	if(Preguntar_Suavizado()) estado = 47;
		// break;

		// 46. Toma de muestra
		case 46:
			estado = Tomar_muestra(estado);
		break;

		// 47. Vaciado de tanque
		case 47:
			if(Vaciado()) estado = 48;
		break;

		// 48. Fin
		case 48:
			Fin_proceso();
			estado = 1;
			return true;
		break;
	}

	return false;
}



int Preblanqueo_quimico(int temperatura, int tiempo){

	static int estado = 1;			// Variable encargada de pasar de un proceso a otro		
	int temp_ok = false;			// Verifica si se llego a la temperatura deseada
	int press_ok = false;			// Verifica si se llego a la presion deseada
	
	static int check_state = 0;			// Se uitiliza para actualizar en la pantalla el paso del proceso
	if (check_state != estado)
	{
		send_msj("nPaso.val=",estado);		// Muestra en pantalla el paso del proceso
		check_state = estado;
	}

	switch(estado)
	{

		// 1. Llenado a nivel 2
		case 1:
			if(Llenado(2)) estado = 2;
		break;

		// 2. Llamado a operador
		case 2:
			if(Llamado_op()) estado = 3;
		break;

		// 3. Adicion rapida de 10 min
		case 3:	
			if(Adicion_rapida(10)) estado = 4;
		break;

		// 4. Circulacion de 10 min
		case 4:
			if(Circulacion(10)) estado = 5;
		break;

		// 5. Subir temperatura a 98°
		case 5:
			temp_ok = Calentamiento(temperatura,2);
			press_ok = Presurizado();
			if ( Temp_actual() >= 98) estado = 6;
		break;

		// 6. Circulacion por 30 min
		case 6:
			Calentamiento(temperatura,2);
			Presurizado();
			if(Circulacion(tiempo))
			{ 
				cerrar_vapor();
				estado = 7;
			}
		break;

		// 7. Enfriamiento a 60°
		case 7:
			temp_ok = Enfriamiento(60,2);
			press_ok = Despresurizado();
			if (Temp_actual() <= 60 && temp_ok && press_ok) estado = 8;
		break;

		// 8. Toma de muestra
		case 8:
			estado = Tomar_muestra(estado);
		break;

		// 9. Lavado por rebose 10 min
		case 9:
			if(Lavado_rebose(10)) estado = 10;
		break;

		// 10. Vaciado de tanque
		case 10:
			if(Vaciado()) estado = 11;
		break;

		// 11. Llenado a nivel 2
		case 11:
			if(Llenado(2)) estado = 12;
		break;

		// 12. Llamado a operador
		case 12:
			if(Llamado_op()) estado = 13;
		break;

		// 13. Adicion rapida de 5 min
		case 13:
			if(Adicion_rapida(5)) estado = 14;
		break;

		// 14. Circulacion por 10 min
		case 14:
			if(Circulacion(15)) estado = 15;
		break;

		// 15. Vaciado de tanque
		case 15:
			if(Vaciado()) estado = 16;
		break;

		// 16. Llenado a nivel 2
		case 16:
			if(Llenado(2)) estado = 17;
		break;

		// 17. Lavado por rebose de 10 min
		case 17:
			if(Lavado_rebose(10)) estado = 18;
		break;

		// 18. Vaciado de tanque
		case 18:
			if(Vaciado()) estado = 19;
		break;

		// 19. Fin
		case 19:
			Fin_proceso();
			estado = 1;
			return true;
		break;

	}

	return false;

}




int Preblanqueo_jabon(int temperatura, int tiempo){

	static int estado = 1;			// Variable encargada de pasar de un proceso a otro		
	int temp_ok = false;			// Verifica si se llego a la temperatura deseada
	int press_ok = false;			// Verifica si se llego a la presion deseada

	static int check_state = 0;			// Se uitiliza para actualizar en la pantalla el paso del proceso
	if (check_state != estado)
	{
		send_msj("nPaso.val=",estado);		// Muestra en pantalla el paso del proceso
		check_state = estado;
	}

	switch(estado)
	{
		// 1. Llenado a nivel 2
		case 1:
			if(Llenado(2)) estado = 2;
		break;

		// 2. Llamado a operador
		case 2:
			if(Llamado_op()) estado = 3;
		break;		

		// 3. Adicion rapida 3 min
		case 3:
			if(Adicion_rapida(3)) estado = 4;
		break;

		// 4. Circulacion de 10 min
		case 4:
			if(Circulacion(10)) estado = 5;
		break;

		// 5. Subir temperatura a 98°
		case 5:
			temp_ok = Calentamiento(temperatura,2);
			press_ok = Presurizado();
			if ( Temp_actual() >= 98 && temp_ok && press_ok) estado = 6;
		break;

		// 6. Circulacion por 30 min
		case 6:
			Calentamiento(temperatura,2);
			Presurizado();
			if(Circulacion(tiempo))
			{	
				cerrar_vapor();
				estado = 7;
			}

		break;

		// 7. Enfriamiento a 60°
		case 7:
			temp_ok = Enfriamiento(60,2);
			press_ok = Despresurizado();
			if (Temp_actual() <= 60 && temp_ok && press_ok) estado = 8;
		break;


		// 8. Lavado por rebose 10 min
		case 8:
			if(Lavado_rebose(10)) estado = 9;
		break;

		// 9. Vaciado de tanque
		case 9:
			if(Vaciado()) estado = 10;
		break;

		// 10. Llenado a nivel 2
		case 10:
			if(Llenado(2)) estado = 11;
		break;

		// 11. Lavado por rebose 5 min
		case 11:
			if(Lavado_rebose(5)) estado = 12;
		break;

		// 12. Vaciado de tanque
		case 12:
			if(Vaciado()) estado = 13;
		break;

		// 13. Fin
		case 13:
			Fin_proceso();
			estado = 1;
			return true;
		break;

	}

	return false;
}



int Saponizado(int temperatura, int tiempo){

	static int estado = 1;			// Variable encargada de pasar de un proceso a otro		
	int temp_ok = false;			// Verifica si se llego a la temperatura deseada
	int press_ok = false;			// Verifica si se llego a la presion deseada

	static int check_state = 0;			// Se uitiliza para actualizar en la pantalla el paso del proceso
	if (check_state != estado)
	{
		send_msj("nPaso.val=",estado);		// Muestra en pantalla el paso del proceso
		check_state = estado;
	}
	
	switch(estado)
	{
		// 1. Llenado a nivel 2
		case 1:
			if(Llenado(2)) estado = 2;
		break;

		// 2. Llamado de operador
		case 2:
			if(Llamado_op()) estado = 3;
		break;

		// 3. Adicion rapida
		case 3:
			if(Adicion_rapida(5)) estado = 4;
		break;

		// 4. Circulacion 10 min
		case 4:
			if(Circulacion(10)) estado = 5;
		break;

		// 5. Llamado de operador
		case 5:
			if(Llamado_op()) estado = 6;
		break;

		// 6. Adicion lenta de 10 min cada 20 seg por 2 seg
		case 6:
			if(Adicion_lenta(10,20,2)) estado = 7;
		break;

		// 7. Subir temperatura a 105° a 1.5gr/min
		case 7:
			temp_ok = Calentamiento(temperatura,1.5);
			press_ok = Presurizado();
			if (Temp_actual() >= temperatura && temp_ok && press_ok) estado = 8;
		break;

		// 8. Circulacion 40 min
		case 8:
			Calentamiento(temperatura,1.5);
			Presurizado();
			if(Circulacion(tiempo)) 
			{	
				cerrar_vapor();
				estado = 9;
			}
		break;	

		// 9. Enfriamiento a 50°
		case 9:
			temp_ok = Enfriamiento(50,1.5);
			press_ok = Despresurizado();
			if (Temp_actual() <= 50 && temp_ok && press_ok) estado = 10;
		break;

		// 10. Llamado de operador (Toma de muestra)
		case 10:
			estado = Tomar_muestra(estado);
		break;

		// 11. Lavado por rebose 10 min
		case 11:
			if(Lavado_rebose(10)) estado = 12;
		break;

		// 12. Vaciado de tanque
		case 12:
			if(Vaciado()) estado = 13;
		break;

		// 13. Llenado a nivel 2
		case 13:
			if(Llenado(2)) estado = 14;
		break;

		// 14. Llamado de operador
		case 14:
			if(Llamado_op()) estado = 15;
		break;	

		// 15. Adicion rapida 3 min
		case 15:
			if(Adicion_rapida(3)) estado = 16;
		break;

		// 16. Circulacion 20 min
		case 16:
			if(Circulacion(20)) estado = 17;
		break;

		// 17. Vaciado de tanque
		case 17:
			if(Vaciado()) estado = 18;
		break; 

		// 18. Llenado a nivel 2
		case 18:
			if(Llenado(2)) estado = 19;
		break;

		// 19. Toma de muestra
		case 19:
			estado = Tomar_muestra(estado);
		break;

		// 20. Subir temperatura a 60°
		case 20:
			temp_ok = Calentamiento(60,2);
			if (Temp_actual() >= 60 && temp_ok) estado = 21;
		break;

		// 21. Circulacion 10 min
		case 21:
			Calentamiento(60,2);
			if(Circulacion(10)) 
			{	
				cerrar_vapor();
				estado = 22;
			}
		break;

		// 22. Vaciado de tanque
		case 22:
			if(Vaciado()) estado = 23;
		break;

		// 23. Fin
		case 23:
			Fin_proceso();
			estado = 1;
			return true;
		break;
	}

	return false;
}



int Directo(int temperatura, int tiempo){

    static int estado = 1;			// Variable encargada de pasar de un proceso a otro		
	int temp_ok = false;			// Verifica si se llego a la temperatura deseada
	int press_ok = false;			// Verifica si se llego a la presion deseada

	static int check_state = 0;			// Se uitiliza para actualizar en la pantalla el paso del proceso
	if (check_state != estado)
	{
		send_msj("nPaso.val=",estado);		// Muestra en pantalla el paso del proceso
		check_state = estado;
	}

	switch(estado)
	{
		// 1. Llenado a nivel 2
		case 1:
			if(Llenado(2)) estado = 2;
		break;


		// 2. Preparacion de tanque
		case 2:
			if(Llamado_op()) estado = 3;
		break;

		// 3. Adicion rapida 5 min
		case 3:
			if(Adicion_rapida(5)) estado = 4;
		break;

		// 4. Circulacion 10 min
		case 4:
			if(Circulacion(10)) estado = 5;
		break;

		// 5. Preparacion de tanque
		case 5:
			if(Llamado_op()) estado = 6;
		break;

		// 6. Adicion lenta de colorante
		case 6:
			if(Adicion_lenta(30,20,2)) estado = 7;
		break;

		// 7. Circulacon 20 min
		case 7:
			if(Circulacion(20)) estado = 8;
		break;

		// 8. Subir temperatura
		case 8:
			temp_ok = Calentamiento(temperatura,2);
			press_ok = Presurizado();
			if (Temp_actual() >= temperatura && temp_ok && press_ok) estado = 9;
		break;

		// 9. Circulacion 
		case 9:
			Calentamiento(temperatura,2);
			if(Circulacion(tiempo)) 
			{	
				cerrar_vapor();
				estado = 10;
			}
		break;

		// 10. Enfriamiento a 60 
		case 10:
			temp_ok = Enfriamiento(60,2);
			press_ok = Despresurizado();
			if (Temp_actual() <= 60 && temp_ok && press_ok) estado = 11;
		break;

		// 11. Toma de muestra
		case 11:
			estado = Tomar_muestra(estado);
		break;

		// 12. Lavado por rebose 10 min
		case 12:
			if(Lavado_rebose(10)) estado = 13;
		break;

		// 13. Vaciado
		case 13:
			if(Vaciado()) estado = 14;
		break;

		// // 11. Suaviazado
		// case 11:
		// 	if(Preguntar_Suavizado()) estado = 12;
		// break;

		// 14. Toma de muestra
		case 14:
			estado = Tomar_muestra(estado);
		break;

		// 15. Fin
		case 15:
			Fin_proceso();
			estado = 1;
			return true;
		break;	
	}

	return false;
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
	static int check_state = 0;			// Se uitiliza para actualizar en la pantalla el paso del proceso
	if (check_state != estado)
	{
		send_msj("nPaso.val=",estado);		// Muestra en pantalla el paso del proceso
		send_msj("nProc.val=",7);
		check_state = estado;
	}

    switch(estado)
    {   
        // 0. Realiza el llenado a nivel 1 si hace falta
        case 0:
            if(Llenado(2)) estado = 1;
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
        	Fin_proceso();
            estado = 0;
            return true;
        break;
    }

    return false;

}



//LAVADO DE MAQUINA, FALTA IMLEMENTAR FUNCION CON VALVULA DE DUCHA
//Y ADEMAS FALTA SABER COMO FUNCIONA CON LA DUCHA

int Lavado_Maquina(){

	static int estado = 1;			// Variable encargada de pasar de un proceso a otro		
	int temp_ok = false;			// Verifica si se llego a la temperatura deseada
	int press_ok = false;			// Verifica si se llego a la presion deseada

	static int check_state = 0;			// Se uitiliza para actualizar en la pantalla el paso del proceso
	if (check_state != estado)
	{
		send_msj("nPaso.val=",estado);		// Muestra en pantalla el paso del proceso
		check_state = estado;
	}

	
	switch(estado)
	{

		// 1. Llenado a nivel 2
		case 1:
			if(Llenado(2)) estado = 2;
		break;

		// 2. Llamado de operador
		case 2:
			if(Llamado_op()) estado = 3;
		break;

		// 3. Adicion rapida de 5 min
		case 3:
			if(Adicion_rapida(5)) estado = 4;
		break;

		// 4. Subir temperatura a 130
		case 4:

			temp_ok = Calentamiento(130,0);
			press_ok = Presurizado();
			if (Temp_actual() >= 130 && temp_ok && press_ok) estado = 5;
		break;

		// 5. Circulacion por 60 min
		case 5:
			Calentamiento(130,0);
			Presurizado();
			if(Circulacion(60))
			{
				cerrar_vapor();
				estado = 6;
			}
		break;


		// 6. Enfriamiento a 60
		case 6:
			temp_ok = Enfriamiento(60,2);
			press_ok = Despresurizado();
			if (Temp_actual() <= 60 && temp_ok && press_ok) estado = 7;
		break;

		// 7. Vaciado
		case 7:
			if(Vaciado()) estado = 8;
		break;

		// 8. Llenado a nivel 2
		case 8:
			if(Llenado(2)) estado = 9;
		break;

		// 9. Preparacion de tanque
		case 9:
			if(Llamado_op()) estado = 10;
		break;

	    // 10. Adicion rapida 5 min
		case 10:
			if(Adicion_rapida(5)) estado = 11;
		break;

		// 11. Circulacion 15 min
		case 11:
			if(Circulacion(15)) estado = 12;
		break;

		// 12. Vaciado
		case 12:
			if(Vaciado()) estado = 13;
		break;

		// 13. Fin
		case 13:
			Fin_proceso();
			estado = 1;
			return true;
		break;

		return false;
	}

}





int Lavado_reductivo(int temperatura, int tiempo){

	static int estado = 1;			// Variable encargada de pasar de un proceso a otro		
	int temp_ok = false;			// Verifica si se llego a la temperatura deseada
	int press_ok = false;			// Verifica si se llego a la presion deseada

	static int check_state = 0;			// Se uitiliza para actualizar en la pantalla el paso del proceso
	if (check_state != estado)
	{
		send_msj("nPaso.val=",estado);		// Muestra en pantalla el paso del proceso
		send_msj("nProc.val=",8);
		check_state = estado;
	}

	switch(estado)
	{
		// 1. Llenado a nivel 2
		case 1:
			if(Llenado(2)) estado = 2;
		break;

		// 2. Adicion rapida 10 min
		case 2:
			if(Adicion_rapida(10)) estado = 3;
		break;

		// 3. Subir temperatura 
		case 3:
			temp_ok = Calentamiento(temperatura,2);
			Presurizado();
			if (Temp_actual() >= temperatura && temp_ok) estado = 4;
		break;

		// 4. Circulacion 
		case 4:
			Calentamiento(temperatura,2);
			if(Circulacion(tiempo))
			{
				cerrar_vapor();
				estado = 5;
			}
		break;

		// 5. Enfriamiento a 60
		case 5:
			temp_ok = Enfriamiento(60,2);
			press_ok = Despresurizado();
			if (Temp_actual() <= 60 && temp_ok) estado = 6;
		break;

		// 6. Toma de muestra
		case 6:	
			estado = Tomar_muestra(estado);
		break;

		// 7. Lavado por rebose 15 min
		case 7:
			if(Lavado_rebose(15)) estado = 8;
		break;

		// 8. Vaciado
		case 8:
			if(Vaciado()) estado = 9;
		break;

		// 9. Llenado a nivel 2
		case 9:
			if(Llenado(2)) estado = 10;
		break;

		// 10. Preparacion de tanque
		case 10:
			if(Llamado_op()) estado = 11;
		break;

		// 11. Adicion rapida 3 min
		case 11:
			if(Adicion_rapida(3)) estado = 12;
		break;

		// 12. Circulacion 10 min
		case 12:	
			if(Circulacion(10)) estado = 13;
		break;

		// 13. Fin
		case 13:
			Fin_proceso();
			estado = 1;
			return true;
		break;
	}

	return false;

}
