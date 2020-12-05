#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void runCache();
void runRAM();
FILE* runAddr();
void blockCheck(FILE* memory);

// Estructura de linea y del proceso por defecto / Struct line of cache and the default proccess

typedef struct{
	short int ETQ;
	short int Datos[8];

}T_LINEA_CACHE;

typedef struct{

	int tiempoglobal;
	int numerofallo;
	T_LINEA_CACHE cache[3];
}nPROCESS;

//Global var

nPROCESS proceso;
unsigned char RAM[1023];
unsigned char memory_access[12];

//Mascara de bits / Bit-mask

#define bit_BLOCK 0b1111111000
#define bit_ETQ 0b1111100000
#define bit_LINE 0b0000011000
#define bit_WORD 0b0000000111


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

//	sleep(1);
	printf("Inicializando la Cache");printf(".");printf(".");printf(".\n");

//	sleep(3);

	//Bucle for para inicializar la linea / for loop to initialize the cache line.
	for(int i=0; i < 4; i++){

		proceso.cache[i].ETQ = final_ETQ;
		printf("La ETQ %X tiene un valor inicial de ", proceso.cache[i].ETQ);

		for(int j=0; j<9; j++){
			proceso.cache[i].Datos[j] = 0;
			printf("%d ", proceso.cache[i].Datos[j]);
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
	//sleep(2);

	proceso.tiempoglobal = 0;

	for(int f = 0; f<12 ; f++){
		proceso.numerofallo = 0;
		addr_block[f] = ( addr[f] & bit_BLOCK ) >> 3;
		addr_ETQ[f] = ( addr[f] & bit_ETQ ) >> 3;
		addr_line[f] = ( addr[f] & bit_LINE ) >> 3;
		addr_word[f] = ( addr[f] & bit_WORD );

		if(addr_ETQ[f] == proceso.cache[addr_line[f]].ETQ){

			if(proceso.tiempoglobal > 0){
				if(addr_ETQ[f] == 0){
					printf("\n T: %d | Fallo de Cache: %d | Addr %04x | ETQ %X     | Linea %02x | Palabra %02x | Bloque %02x  | ---> Son Iguales.", proceso.tiempoglobal, proceso.numerofallo, addr[f], addr_ETQ[f], addr_line[f], addr_word[f], addr_block[f]); //the are the same line.
				}else{
					printf("\n T: %d  | Fallo de Cache: %d | Addr %04x | ETQ %X   | Linea %02x | Palabra %02x | Bloque %02x  | ---> Son Iguales.", proceso.tiempoglobal, proceso.numerofallo, addr[f], addr_ETQ[f], addr_line[f], addr_word[f], addr_block[f]); //the are the same line.
				}
			}else{
				if(addr_ETQ[f] == 0){
					printf("\n T: %d  | Fallo de Cache: %d | Addr %04x | ETQ %X    | Linea %02x | Palabra %02x | Bloque %02x  | ---> Son Iguales.", proceso.tiempoglobal, proceso.numerofallo, addr[f], addr_ETQ[f], addr_line[f], addr_word[f], addr_block[f]); //the are the same line.
				}else{
						printf("\n T: %d  | Fallo de Cache: %d | Addr %04x | ETQ %X   | Linea %02x | Palabra %02x | Bloque %02x  | ---> Son Iguales.", proceso.tiempoglobal, proceso.numerofallo, addr[f], addr_ETQ[f], addr_line[f], addr_word[f], addr_block[f]); //the are the same line.
				}
			}
		//	sleep(1);
		}else{
			proceso.numerofallo++;

			if(proceso.tiempoglobal > 0){

				if(proceso.tiempoglobal > 99){

					if(addr_ETQ[f] == 0){
						printf("\n T: %d| Fallo de Cache: %d | Addr %04x | ETQ %X    | Linea %02x | Palabra %02x | Bloque %02x |", proceso.tiempoglobal, proceso.numerofallo, addr[f], addr_ETQ[f], addr_line[f], addr_word[f], addr_block[f]);
					}else{
						printf("\n T: %d| Fallo de Cache: %d | Addr %04x | ETQ %X   | Linea %02x | Palabra %02x | Bloque %02x |", proceso.tiempoglobal, proceso.numerofallo, addr[f], addr_ETQ[f], addr_line[f], addr_word[f], addr_block[f]); 
					}

				}else{

					if(addr_ETQ[f] == 0){
						printf("\n T: %d | Fallo de Cache: %d | Addr %04x | ETQ %X    | Linea %02x | Palabra %02x | Bloque %02x |", proceso.tiempoglobal, proceso.numerofallo, addr[f], addr_ETQ[f], addr_line[f], addr_word[f], addr_block[f]); 
					}else{
						printf("\n T: %d | Fallo de Cache: %d | Addr %04x | ETQ %X   | Linea %02x | Palabra %02x | Bloque %02x |", proceso.tiempoglobal, proceso.numerofallo, addr[f], addr_ETQ[f], addr_line[f], addr_word[f], addr_block[f]); 
					}

				}
			}else{

				if(addr_ETQ[f] == 0){
					printf("\n T: %d  | Fallo de Cache: %d | Addr %04x | ETQ %X     | Linea %02x | Palabra %02x | Bloque %02x |", proceso.tiempoglobal, proceso.numerofallo, addr[f], addr_ETQ[f], addr_line[f], addr_word[f], addr_block[f]);
				}else{
					printf("\n T: %d  | Fallo de Cache: %d | Addr %04x | ETQ %X   | Linea %02x | Palabra %02x | Bloque %02x |", proceso.tiempoglobal, proceso.numerofallo, addr[f], addr_ETQ[f], addr_line[f], addr_word[f], addr_block[f]); 
				}
			}
			proceso.tiempoglobal+=10;
			sleep(1);
		}

	}

	printf("\n");

}
