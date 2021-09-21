#include <Arduino.h>
#include "Pines.h"
#include "procesos.h"

// Prototipo de funcion que pregunta si se desea suavizado o no mediante el hmi
int preguntar(); 



int Poliester(int temperatura){

	static int estado = 1;
	//int Temp_actual;

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

		// 3. Adicion rapida
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
				if (Temp_actual() < 89) Calentamiento(90,2);

				else if ( Temp_actual() >= 88) Calentamiento(130,1.5);
				
			}
			else Calentamiento(temperatura,2);
			
			Presurizado();
			if ( Temp_actual() >= temperatura - 2) estado = 9;

		break;

		// 9. Circulacion por 1 hora y se asegura de mantener la temperatura deseada
		case 9:
			if (Circulacion(60)) estado = 10;
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

		// 16. Fin
		case 16:
			estado = 1;
			return true;
		break;	
	}

	return false;
}


// VERIFICAR LINEA 272 SI NO LLEVA GRADIENTE
int Algodon(int temperatura){

	static int estado = 1;
	int suave; // indicador de suavizado
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
			Calentamiento(temperatura,1.5);
			if ( Temp_actual() >= temperatura - 2) estado = 8;
		break;

		// 8. Llamado a operador
		case 8:
			if(Llamado_op()) estado = 9;
		break;

		// 9. Adicion lenta (Alkali) de 5 min cada 10 seg por 2 seg
		case 9:
			if(Adicion_lenta(5,10,2)) estado = 10;
		break;

		// 10. Adicion lenta (Alkali) de 25 min cada 10 seg por 3 seg
		case 10:
			if(Adicion_lenta(25,10,3)) estado = 11;
		break;

		// 11. Adicion lenta (Alkali) 15 min cada 15 seg por 3 seg
		case 11:
			if(Adicion_lenta(15,15,3)) estado = 12;
		break;

		// 12. Circulacion por 1 hora y manterner en 60°
		case 12:
			if (Circulacion(60)) estado = 13;
			Calentamiento(temperatura,1.5);
		break;

		// 13. Llamado a operador (Tomar muestra)
		case 13:
			if(Llamado_op()) estado = 14;
		break;

		// 14. Lavado por rebose de 10 min
		case 14:
			if(Lavado_rebose(10)) estado = 15;
		break;

		// 15. Vaciado de tanque
		case 15:
			if(Vaciado()) estado = 16;
		break;

		//16. Llenado a nivel 1
		case 16:
			if(Llenado(1)) estado = 17;
		break;

		// 17. Circulacion por 10 min
		case 17:
			if(Circulacion(10))	estado = 18;
		break;

		// 18. Vaciado de tanque
		case 18:
			if(Vaciado()) estado = 19;
		break; 

		// 19. Llenado a nivel 1
		case 19:
			if(Llenado(1)) estado = 20;
		break;

		// 20. Llamado a operador
		case 20:
			if(Llamado_op()) estado = 21;
		break;

		// 21. Adicion rapida de 3 min
		case 21:
			if(Adicion_rapida(3)) estado = 22;
		break;

		// 22. Circulacion por 5 min
		case 22:
			if(Circulacion(5)) estado = 23;
		break;

		// Enjabonado en caliente
		// 23. Subir temperatura a 90° a 2°/min
		case 23:
			Calentamiento(90,2);
			Presurizado();
			if ( Temp_actual() >= 90) estado = 24;
		break;

		// 24. Circulacion por 20 min
		case 24:
			if(Circulacion(20)) estado = 25;
			Calentamiento(90,2);
			Presurizado();
		break;

		// 25. Enfriamiento a 60° a 2°/min
		case 25:
			Enfriamiento(60,2);
			Despresurizado();
			if (Temp_actual() <= 60) estado = 26;
		break;

		// 26. Vaciado de tanque
		case 26:
			if(Vaciado()) estado = 27;
		break;

		// 27. Llenado a nivel 1
		case 27:
			if(Llenado(1)) estado = 28;
		break;

		// 28. Subir temperatura a 65°
		case 28:
			Calentamiento(65,0);
			if ( Temp_actual() >= 61) estado = 29;
		break;

		// 29. Circulacion por 10 min
		case 29:
			if(Circulacion(10)) estado = 30;
		break;

		// 30. Vaciado de tanque
		case 30:
			if(Vaciado()) estado = 31;
		break;

		//  31. Llenado a nivel 1
		case 31:
			if(Llenado(1)) estado = 32;
		break; 

		// 32. Circulacion por 10 min
		case 32:
			if(Circulacion(10)) estado = 33;
		break;

		// 33. Vaciado de tanque
		case 33:
			if(Vaciado()) estado = 34;
		break;

		// 34. Llenado a nivel 1
		case 34:
			if(Llenado(1)) estado = 35;
		break;

		// 35. Lavado por rebose de 15 min
		case 35:
			if(Lavado_rebose(15)) estado = 36;
		break; 

		// 36. Vaciado de tanque
		case 36:
			if(Vaciado()) estado = 37;
		break;

		// 37. Llenado a nivel 1
		case 37:
			if(Llenado(1)) estado = 38;
		break;

		// 38. Llamado a operador
		case 38:
			if(Llamado_op()) estado = 39;
		break;	

		// 39. Adicion rapida
		case 39:
			if(Adicion_rapida(5)) estado = 40;
		break;

		// 40. Llamado a operador
		case 40:
			if(Llamado_op()) estado = 41;
		break;	

		// 41. Adicion rapida
		case 41:
			if(Adicion_rapida(5)) estado = 42;
		break;

		// 42. Circulacion por 20 min
		case 42:
			if(Circulacion(20)) estado = 43;
		break; 

		// Parte nueva añadida (el suavizado debe ser opcional)
		// 43. preguntar si se desa suvizado de tela (Suavizado es opcional)
		case 43:
			if(preguntar()) estado = 44;
			else estado = 45;
		break;

		// 44. Suavizado
		case 44:
			if(Suavizado()) estado = 45;
		break;

		// 45. Llamado de operador (Toma de muestra)
		case 45:
			if(Llamado_op()) estado = 46;
		break;

		// 46. Vaciado de tanque
		case 46:
			if(Vaciado()) estado = 47;
		break;

		// 47. Fin
		case 47:
			estado = 1;
			return true;
		break;
	}

	return false;
}



int Preblanqueo_quimico(){

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

