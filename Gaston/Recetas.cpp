#include <Arduino.h>
#include "Pines.h"
#include "procesos.h"






int Poliester(int temperatura){

	static int estado = 1;
	//int Temp_actual;

	switch (estado)
	{
		// 1. Llenado
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

		// 7. Circulacion

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
			else Calentamiento(temperatura,1.5);
			
			Presurizado();
			if ( Temp_actual() >= temperatura - 2) estado = 9;

		break;

		// 9. Circulacion por 1 hora y se asegura de mantener la temperatura deseada
		case 9:
			if (Circulacion(60)) estado = 10;
			Calentamiento(temperatura,1.5);
			Presurizado();	
		break;

		// 10. Enfriamiento
		case 10:
			Enfriamiento(60,2);
			Despresurizado();
			if (Temp_actual() <= 60) estado = 11;
		break;

		// 11. Lavado por rebose de 10 min
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
