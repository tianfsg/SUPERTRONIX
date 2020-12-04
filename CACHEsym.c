#include <stdio.h>
#include <stdlib.h>

void runCache();
void runRAM();
void runAddr();

int tiempoglobal = 0;
int numerofallo = 0;

typedef struct{
	short int ETQ;
	short int Datos[8];

}T_LINEA_CACHE;

T_LINEA_CACHE linea[3];
unsigned char RAM[1023];
int memory_access[12];

int main (){


	unsigned int a = 0x02E4;

	printf("Direccion (1) HEX: %#.04x | DEC: %d\n", a,a);


	runCache();
	runRAM();
	runAddr();

	return 0;
}

//Funciones que inicializan RAM, Cache, Dirrecciones / Functions where initialize the Cache, RAM and Address sets.

void runCache(){

	const short int final_ETQ = 0xFF;
	
	//Bucle for para inicializar la linea / for loop to initialize the cache line.
	for(int i=0; i < 4; i++){

		linea[i].ETQ = final_ETQ;
		printf("La ETQ %X tiene un valor inicial de ", linea[i].ETQ);

		for(int j=0; j<9; j++){
			linea[i].Datos[j] = 0;
			printf("%d ", linea[i].Datos[j]);
		}
		printf("\n");
	}
}

void runRAM(){

	FILE *bin;

	bin = fopen("RAM.bin", "r");

	if(&bin == NULL){
		printf("El ficher 'RAM.bin' no se pudo abrir\n\n");

		exit(-1);
	}

	fseek(bin, 0, SEEK_SET);
	fread(RAM, sizeof(RAM), 1, bin); 


//Debug

//	for(int k=0; k<1024; k++){
//		printf("Posicion de K(%d) value: %u \n", k, RAM[k]);
//	}

	fclose(bin);
}


void runAddr(){

	//Open the FILE

	FILE *memory;

	memory = fopen("accesos_memoria1.txt", "r");

	rewind(memory);

	//If the file doesn't exist it breaks 
	if(memory == NULL){
		printf("El ficher 'accesos_memoria.txt' no se pudo abrir");
		exit(-1);
	}

	//Read the content and buffer it on 'memory_access'

	fread(memory_access,sizeof(memory_access), 1, memory);

//Debug

	for(int j=0; j < 12 ; j++){
		printf("Direccion (%d) HEX: %#.04x | DEC: %d\n", j+1, *memory_access[j], *memory_access[j]); 
	}

	fclose(memory);
}
