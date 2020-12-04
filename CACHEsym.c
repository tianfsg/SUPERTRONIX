#include <stdio.h>
#include <math.h>

int tiempoglobal = 0;
int numerofallo = 0;

typedef struct{
	short int ETQ;
	short int Datos[8];

}T_LINEA_CACHE;


int main (){

	unsigned char RAM[1023];
	T_LINEA_CACHE linea[3];

	for(int i=0; i<4 ; i++){
		linea[i].ETQ = 16;
		printf("valor de la %d ETQ %d \n", i,linea[i].ETQ);
		for(int j=0; j < 9; j++){
			linea[i].Datos[j] = 0;
			printf("El valor %d con los valores %d \n", j, linea[i].Datos[j]);
		}
	}


	return 0;
}
