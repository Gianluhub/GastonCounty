#include <Arduino.h>
#include "Pines.h"
#include "procesos.h"

// Prototipo de funcion que pregunta si se desea suavizado o no mediante el hmi
int preguntar(){return false;}



int Poliester(int temperatura, int tiempo){

	static int estado = 1;
	//int Temp_actual;
	if(digitalRead(Start)>=HIGH) estado+=1;

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

		// 3. Adicion rapida 4 min
		case 3:
			if(Adicion_rapida(4)) estado = 4;
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
				if (Temp_actual() < 90) Calentamiento(90,2);

				else if ( Temp_actual() >= 90) Calentamiento(130,1.5);
				
			}
			else Calentamiento(temperatura,2);
			
			Presurizado();
			if ( Temp_actual() >= temperatura - 1) estado = 9;

		break;

		// 9. Circulacion por 1 hora y se asegura de mantener la temperatura deseada
		case 9:
			if (Circulacion(tiempo)) estado = 10;
			Calentamiento(temperatura,2);
			Presurizado();	
		break;

		// 10. Enfriamiento
		case 10:
			Enfriamiento(60,2);
			Despresurizado();
			if (Temp_actual() <= 60) estado = 11;
		break;

		// 11. Lavado por rebose de 15 min
		case 11:
			if (Lavado_rebose(15)) estado = 12;
		break;

		// 12. Vaciado de tanque
		case 12:
			if (Vaciado()) estado = 13;
		break;

		// 13. Llenado a nivel 2
		case 13:
			if(Llenado(2)) estado = 14;
		break;

		// 14. Lavado por rebose de 15 min
		case 14:
			if(Lavado_rebose(15)) estado = 15;
		break;

		// 15. Vaciado
		case 15:
			if (Vaciado()) estado = 16;
		break;

		// Parte nueva añadida (el suavizado debe ser opcional)
		// 16. Preguntar por suavizado
		case 16:
			if(preguntar()) estado = 17;
			else estado = 18;
		break;

		// 17. Suavizado
		case 17:
			if(Suavizado()) estado = 18;
		break;

		// 18. Fin
		case 18:
			estado = 1;
			return true;
		break;	
	}

	return false;
}


// VERIFICAR LINEA 272 SI NO LLEVA GRADIENTE
int Algodon(int temperatura, int tiempo){

	static int estado = 1;
	int suave; // indicador de suavizado
	if(digitalRead(Start)>=HIGH) estado+=1;
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
			Calentamiento(temperatura,2);
			if (Temp_actual() >= temperatura - 1) estado = 8;
		break;

		// 8. Circulacion
		case 8:
			if(Circulacion(tiempo)) estado = 9;
			Calentamiento(temperatura,2);
		break;

		// 9. Llamado a operador
		case 9:
			if(Llamado_op()) estado = 10;
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

		// 13. Circulacion por 1 hora y manterner en 60°
		case 13:
			if (Circulacion(60)) estado = 14;
			Calentamiento(temperatura,1.5);
		break;

		// 14. Llamado a operador (Tomar muestra)
		case 14:
			if(Llamado_op()) estado = 15;
		break;

		// 15. Lavado por rebose de 10 min
		case 15:
			if(Lavado_rebose(10)) estado = 16;
		break;

		// 16. Vaciado de tanque
		case 16:
			if(Vaciado()) estado = 17;
		break;

		//17. Llenado a nivel 1
		case 17:
			if(Llenado(1)) estado = 18;
		break;

		// 18. Circulacion por 10 min
		case 18:
			if(Circulacion(10))	estado = 19;
		break;

		// 19. Vaciado de tanque
		case 19:
			if(Vaciado()) estado = 20;
		break; 

		// 20. Llenado a nivel 1
		case 20:
			if(Llenado(1)) estado = 21;
		break;

		// 21. Llamado a operador
		case 21:
			if(Llamado_op()) estado = 22;
		break;

		// 22. Adicion rapida de 3 min
		case 22:
			if(Adicion_rapida(3)) estado = 23;
		break;

		// 23. Circulacion por 5 min
		case 23:
			if(Circulacion(5)) estado = 24;
		break;

		// Enjabonado en caliente
		// 24. Subir temperatura a 90° a 2°/min
		case 24:
			Calentamiento(90,2);
			Presurizado();
			if (Temp_actual() >= 90) estado = 25;
		break;

		// 25. Circulacion por 20 min
		case 25:
			if(Circulacion(20)) estado = 26;
			Calentamiento(90,2);
			Presurizado();
		break;

		// 26. Enfriamiento a 60° a 2°/min
		case 26:
			Enfriamiento(60,2);
			Despresurizado();
			if (Temp_actual() <= 60) estado = 27;
		break;

		// 27. Vaciado de tanque
		case 27:
			if(Vaciado()) estado = 28;
		break;

		// 28. Llenado a nivel 1
		case 28:
			if(Llenado(1)) estado = 29;
		break;

		// 29. Subir temperatura a 65°
		case 29:
			Calentamiento(65,2);
			if ( Temp_actual() >= 61) estado = 30;
		break;

		// 30. Circulacion por 10 min
		case 30:
			if(Circulacion(10)) estado = 31;
		break;

		// 31. Vaciado de tanque
		case 31:
			if(Vaciado()) estado = 32;
		break;

		//  32. Llenado a nivel 1
		case 32:
			if(Llenado(1)) estado = 33;
		break; 

		// 33. Circulacion por 10 min
		case 33:
			if(Circulacion(10)) estado = 34;
		break;

		// 34. Vaciado de tanque
		case 34:
			if(Vaciado()) estado = 35;
		break;

		// 35. Llenado a nivel 1
		case 35:
			if(Llenado(1)) estado = 36;
		break;

		// 36. Lavado por rebose de 15 min
		case 36:
			if(Lavado_rebose(15)) estado = 37;
		break; 

		// 37. Vaciado de tanque
		case 37:
			if(Vaciado()) estado = 38;
		break;

		// 37. Llenado a nivel 1
		case 38:
			if(Llenado(1)) estado = 39;
		break;

		// 39. Llamado a operador
		case 39:
			if(Llamado_op()) estado = 40;
		break;	

		// 40. Adicion rapida
		case 40:
			if(Adicion_rapida(5)) estado = 41;
		break;

		// 41. Llamado a operador
		case 41:
			if(Llamado_op()) estado = 42;
		break;	

		// 42. Adicion rapida 5 min
		case 42:
			if(Adicion_rapida(5)) estado = 43;
		break;

		// 43. Circulacion por 20 min
		case 43:
			if(Circulacion(20)) estado = 44;
		break; 

		// Parte nueva añadida (el suavizado debe ser opcional)
		// 44. preguntar si se desea suvizado de tela (Suavizado es opcional)
		case 44:
			if(preguntar()) estado = 45;
			else estado = 46;
		break;

		// 45. Suavizado
		case 45:
			if(Suavizado()) estado = 46;
		break;

		// 46. Llamado de operador (Toma de muestra)
		case 46:
			if(Llamado_op()) estado = 47;
		break;

		// 47. Vaciado de tanque
		case 47:
			if(Vaciado()) estado = 48;
		break;

		// 48. Fin
		case 48:
			estado = 1;
			return true;
		break;
	}

	return false;
}



int Preblanqueo_quimico(){

	static int estado = 1;
	if(digitalRead(Start)>=HIGH) estado+=1;
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
			Calentamiento(98,2);
			Presurizado();
			if ( Temp_actual() >= 97) estado = 6;
		break;

		// 6. Circulacion por 30 min
		case 6:
			if(Circulacion(30)) estado = 7;
			Calentamiento(98,2);
			Presurizado();
		break;

		// 7. Enfriamiento a 60°
		case 7:
			Enfriamiento(60,2);
			Despresurizado();
			if (Temp_actual() <= 60) estado = 8;
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

		// 11. Llamado a operador
		case 11:
			if(Llamado_op()) estado = 12;
		break;

		// 12. Adicion rapida de 15 min
		case 12:
			if(Adicion_rapida(15)) estado = 13;
		break;

		// 13. Circulacion por 10 min
		case 13:
			if(Circulacion(10)) estado = 14;
		break;

		// 14. Vaciado de tanque
		case 14:
			if(Vaciado()) estado = 15;
		break;

		// 15. Llenado a nivel 2
		case 15:
			if(Llenado(2)) estado = 16;
		break;

		// 16. Lavado por rebose de 10 min
		case 16:
			if(Lavado_rebose(10)) estado = 17;
		break;

		// 17. Vaciado de tanque
		case 17:
			if(Vaciado()) estado = 18;
		break;

		// 18. Fin

		case 18:
			estado = 1;
			return true;
		break;

	}

	return false;

}




int Preblanqueo_jabon(){

	static int estado = 1;

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
			Calentamiento(98,2);
			Presurizado();
			if ( Temp_actual() >= 98 - 1) estado = 6;
		break;

		// 6. Circulacion por 30 min
		case 6:
			if(Circulacion(30)) estado = 7;
			Calentamiento(98,2);
			Presurizado();

		break;

		// 7. Enfriamiento a 60°
		case 7:
			Enfriamiento(60,2);
			Despresurizado();
			if (Temp_actual() <= 60) estado = 8;
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
			estado = 1;
			return true;
		break;

	}

	return false;
}



int Saponizado(){

	static int estado = 1;

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
			Calentamiento(105,1.5);
			Presurizado();
			if (Temp_actual() >= 104) estado = 8;
		break;

		// 8. Circulacion 40 min
		case 8:
			if(Circulacion(40)) estado = 9;
			Calentamiento(105,1.5);
			Presurizado();
		break;	

		// 9. Enfriamiento a 50°
		case 9:
			Enfriamiento(50,1.5);
			Despresurizado();
			if (Temp_actual() <= 50) estado = 10;
		break;

		// 10. Llamado de operador (Toma de muestra)
		case 10:
			if(Llamado_op()) estado = 11;
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

		// 19. Subir temperatura a 60°
		case 19:
			Calentamiento(60,2);
			if (Temp_actual() >= 60) estado = 20;
		break;

		// 20. Circulacion 10 min
		case 20:
			if(Circulacion(10)) estado = 21;
			Calentamiento(60,2);
		break;

		// 21. Vaciado de tanque
		case 21:
			if(Vaciado()) estado = 22;
		break;

		// 22. Fin
		case 22:
			estado = 1;
			return true;
		break;
	}

	return false;
}

