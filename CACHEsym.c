#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void runCache();
void runRAM();
FILE* runAddr();
void blockCheck(FILE* memory);

// variables globales de Fallo y Tiempo / Global var for globaltime n number of miss.

int tiempoglobal = 0;
int numerofallo = 0;

// Estructura de linea / Struct line of cache

typedef struct{
	short int ETQ;
	short int Datos[8];

}T_LINEA_CACHE;

//Global var

T_LINEA_CACHE linea[3];
unsigned char RAM[1023];
unsigned char memory_access[12];

//Mascara de bits / Bit-mask

#define bit_BLOCK 0b1111111000
#define bit_ETQ 0b1111100000
#define bit_LINE 0b0000011000
#define bit_WORD 0b00000000111


int main (){

	FILE* memory = runAddr();
	runCache();
	runRAM();

	blockCheck(memory);

	fclose(memory);
	return 0;
}

//Funciones que inicializan RAM, Cache, Dirrecciones / Functions where initialize the Cache, RAM and Address sets.

void runCache(){

	const short int final_ETQ = 0xFF;

	sleep(1);
	printf("Inicializando la Cache");printf(".");printf(".");printf(".\n");

	sleep(3);

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

	FILE* bin;

	bin = fopen("RAM.bin", "r");

	if(bin == NULL){
		int H = 0;
		printf("\nEl fichero 'RAM.bin' no existe.\n\n");

		sleep(2);
		H=1;
		do{exit (-1);}while(H==1);
	}

	fseek(bin, 0, SEEK_SET);
	fread(RAM, sizeof(RAM), 1, bin);


//Debug

//	for(int k=0; k<1024; k++){
//		printf("Posicion de K(%d) value: %u \n", k, RAM[k]);
//	}

	fclose(bin);
}


FILE* runAddr(){

	// Abrimos el fichero / Open the FILE

	FILE *memory;
	memory = fopen("accesos_memoria.txt", "r");

	rewind(memory);

	//Si el fichero no existe 'segmention fault error' / If the file doesn't exist it breaks 
	if(memory == NULL){
		int H = 0;
		printf("\nEl fichero 'accesos_memoria.txt' no existe.\n\n");

		sleep(2);
		H=1;
		do{exit (-1);}while(H==1);
	}

	return memory;
}

void blockCheck(FILE *memory){
	//Declaramos e Inicializamos a 0 direccion, y la dirreccion de cada elemento dentro de la cache / Declare and initialize to 0 every address from cache.

	int addr[12], addr_block[12], addr_ETQ[12], addr_line[12], addr_word[12];

	for(int y=0 ; y<12 ; y++){

		addr[y] = 0;
		addr_block[y] = 0;
		addr_ETQ[y] = 0;
		addr_line[y] = 0;
		addr_word[y] = 0;
	}
	//Escaneamos cada linea de las direcciones del fichero / Scan every address from the file.
	for(int z = 0; z<12; z++){
		fscanf(memory, "%X", &addr[z]);
	//debug
	//	printf("addr %#04x \n", addr[z]);
	}


	//Comparamos con el operador logico AND si se corresponde a la mascara de Bits / Compare the addr with the logic operator AND using the mask bit of every element.
	printf("\nEscaneando los bloques..\n");
	sleep(2);
	for(int f = 0; f<12 ; f++){

		addr_block[f] = addr[f] & bit_BLOCK;
		addr_ETQ[f] = addr[f] & bit_ETQ;
		addr_line[f] = addr[f] & bit_LINE;
		addr_word[f] = addr[f] & bit_WORD;

		if(addr_ETQ[f] == linea[addr_line[f]].ETQ){

			printf("\n T: %d | Fallo de Cache: %d | Addr %04x | ETQ %x   | Linea %02x | Palabra %02x | Bloque %02x  | ---> Son Iguales.", tiempoglobal, numerofallo, addr[f], addr_ETQ[f], addr_line[f], addr_word[f], addr_block[f]); //the are the same line.
			sleep(1);
		}else{
			numerofallo++;
			printf("\n T: %d | Fallo de Cache: %d | Addr %04x | ETQ %x | Linea %02x | Palabra %02x | Bloque %02x |", tiempoglobal, numerofallo, addr[f], addr_ETQ[f], addr_line[f], addr_word[f], addr_block[f] );
			tiempoglobal+=10;
			sleep(1);
		}
		numerofallo = 0;
		tiempoglobal = 0;
	}

	printf("\n");

}
