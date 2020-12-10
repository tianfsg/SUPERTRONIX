//
//	Copyright 2020, Sebastian F. Gutierrez R. and Jorge Martinez H. All rights reserved.
//
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

// prototipos de funciones

void runRAM();
FILE* runAddr();
void runCache();
void mostrarCache();
void contenidoCache();
void blockCheck(FILE* memory);
void updateLine(int addr_ETQmemory, int linea, int addr_BLOCKmemory, unsigned char addr_bin[]);

typedef struct{ 				//estructura de la linea; tiene etiqueta y 8 palabras. / Struct LINE; contains a LABEL and 8 WORDS.

	short int ETQ;
	short int palabras[8];

}T_LINEA_CACHE;

typedef struct{			 		//estructura del proceso; tiene tiempoglobal, numero de fallos y 4 lineas de cache. / Struct "Process"; has globaltime, nMiss, and 4 LINES from the Struct LINE.

	int tiempoglobal;
	float tiempomedio;
	int numerofallo;
	int naccesos;
	T_LINEA_CACHE cache[4];

}nPROCESS;

//Variables globales / Global Var

nPROCESS proceso;		 		//Variable proceso; es una estructura y uno de sus elementos es otra estructura (T_LINEA_CACHE) / Global Process var;
int cuentatiempo;
unsigned char RAM[1024];	 	//Aqui ira el contenido de 'RAM.bin' / Global array to save the data of file 'RAM.bin'

//Mascaras de bits / bit-Mask

#define bit_BLOCK 0b1111111000 	 //Bloque / BLOCK
#define bit_ETQ 0b1111100000 	 //Etiqueta / LABEL
#define bit_LINE 0b0000011000	 //Linea / LINE
#define bit_WORD 0b0000000111 	 //Palabra / WORD


int main (){

	FILE* memory;

	memory = runAddr(); 		//asignas el contenido de accesos_memoria.txt a la variable memory / Save the content form file 'accesos_memoria.txt' with func 'runAddr()'

	runRAM(); 					//Asignas el contenido de RAM.bin al array global RAM / Save the content of file 'RAM.bin' with func 'runRAM()' into global array RAM

	blockCheck(memory);
	fclose(memory);

	contenidoCache();			//Muestra el contenido volcado en la cache en caracteres / Show the content of cache in characters.
	printf("\nNumero de accesos: %d\nFallos totales: %d\nTiempo total: %ds\nTiempo medio %.02fs\n\nEl Proceso ha terminado...\n\n", proceso.naccesos, proceso.numerofallo, proceso.tiempoglobal, proceso.tiempomedio);
	
	sleep(2);
	return 0;
}

//Funciones que inicializan RAM, Cache, Dirrecciones / Functions where initialize the Cache, RAM and Address sets.

void runCache(){

	const short int final_ETQ = 0xFF; //Valor inicial de las Etiquetas, FF (HEX) / Constant var LABEL with FF (HEX)

	
	//sleep(0.5);
	printf("Inicializando la Cache...\n\n");

	
	//sleep(1.7);
	
	//Bucle for para inicializar la linea / For-loop to initialize the cache line.
	for(int i=0; i < 4; i++){
		
		proceso.cache[i].ETQ = final_ETQ; //Asignas FF a todas las etiqueta / Initialize LABEL to FF with Constant var
		printf("ETQ %X DATOS: ", proceso.cache[i].ETQ);

		for(int j=0; j<8; j++){ //Asignas 0 a todas las palabras / Initialize every WORD to 0

			proceso.cache[i].palabras[j] = 0x0;
			printf("%d0 ", proceso.cache[i].palabras[j]);

		}
		printf("\n");
	}
}

void runRAM(){ //Asignas el contenido de RAM.bin a la variable 'bin' / Pointer FILE* to 'RAM.bin' called 'bin'

	FILE* bin;

	bin = fopen("RAM.bin", "r");

	if(bin == NULL){ //Manejo de excepciones a la hora de abrir el fichero / Control-if the FILE doesn't exist return exit(-1)

		int H = 0;
		printf("\nEl fichero 'RAM.bin' no existe.\n\n");


		//sleep(1.5);
		H=1;
		do{exit (-1);}while(H==1);

	}
	rewind(bin);

	fread(RAM, sizeof(RAM), 1, bin);

	fclose(bin);
}


FILE* runAddr(){ //Asignas el contenido de 'accesos_memoria.txt' a la variable memory / Pointer FILE* to 'accesos_memoria.txt' called 'memory'

	// Abrimos el fichero / Open the FILE

	FILE *memory;
	memory = fopen("accesos_memoria.txt", "r");

	if(memory == NULL){ //Manejo de excepciones a la hora de abrir el fichero / Control-if the FILE doesn't exist return exit(-1)
		int H = 0;
		printf("\nEl fichero 'accesos_memoria.txt' no existe.\n\n");


        //sleep(1.5);
		H=1;
		do{exit (-1);}while(H==1);
	}

	rewind(memory);

	return memory;
}

void blockCheck(FILE* memory){

	//Declaramos e Inicializamos a 0 direccion, y la dirreccion de cada elemento dentro de la cache / Declare and initialize to 0 every address from cache.

	int addr_memory[12], addr_block[12], addr_ETQ[12], addr_line[12], addr_word[12];

	for(int y=0 ; y<12 ; y++){
		addr_block[y] = 0;
		addr_ETQ[y] = 0;
		addr_line[y] = 0;
		addr_word[y] = 0;
	}

	//Escaneamos cada linea de las direcciones del fichero / Scan every address from the FILE.

	for(int y=0; y<12; y++){
		fscanf(memory, "%X", &addr_memory[y]);;
	}

	//Comparamos con el operador logico AND si se corresponde a la mascara de Bits / Compare the addr with the logic operator AND using the mask bit of every element.

	for(int z = 0; z<12; z++){
		addr_block[z] = ( addr_memory[z] & bit_BLOCK ) >> 3; //Analizar el bloque de cada direccion y despues compararla con RAM.
		addr_ETQ[z] = ( addr_memory[z] & bit_ETQ ) >> 5;
		addr_line[z] = ( addr_memory[z] & bit_LINE ) >> 3;
		addr_word[z] = ( addr_memory[z] & bit_WORD );
	}
	
	runCache();

	printf("\nEscaneando los bloques..\n");
	
	//sleep(2.5);

	proceso.tiempoglobal = 0;
	proceso.numerofallo = 0;

    //Comprobacion / Checking

	for(int f = 0; f < 12; f++){

		if(proceso.cache[addr_line[f]].ETQ != addr_ETQ[f]){
			
			proceso.numerofallo++;						//Aumentamos un fallo / Adding a nMiss
			printf("\nT: %d | Fallo de CACHE %d | ADDR %04X | BLOQUE %02X | ETQ %02X | LINEA %02X | PALABRA %02X |\n", proceso.tiempoglobal, proceso.numerofallo, addr_memory[f], addr_block[f], addr_ETQ[f], addr_line[f], addr_word[f]);
			printf("\nCargando el Bloque %02X en la Linea %02X..\n", addr_block[f], addr_line[f]);

			//sleep(1.5);		
			updateLine(addr_ETQ[f], addr_line[f], addr_block[f], RAM);

			if(proceso.cache[addr_line[f]].ETQ != addr_ETQ[f]){
			}else{
				printf("\nT: %d | Acierto de CACHE | ADDR %04X | ETQ %X | LINEA %02X | PALABRA %02X | DATO %02X |\n", proceso.tiempoglobal, addr_memory[f], addr_ETQ[f], addr_line[f], addr_word[f], proceso.cache[addr_line[f]].palabras[addr_word[f]]);
				proceso.tiempoglobal++;
				cuentatiempo++;

			    //sleep(1.5);		
				mostrarCache();
			}
			
		}else{
			printf("\nT: %d | Acierto de CACHE | ADDR %04X | ETQ %X | LINEA %02X | PALABRA %02X | DATO %02X |\n", proceso.tiempoglobal, addr_memory[f], addr_ETQ[f], addr_line[f], addr_word[f], proceso.cache[addr_line[f]].palabras[addr_word[f]]);
			proceso.tiempoglobal++;
			cuentatiempo++;
			
			//sleep(1.5);
			mostrarCache();
		}

		proceso.naccesos = f+1;
	}
	printf("-----------------------------------------------------------\n");
	proceso.tiempomedio = proceso.tiempoglobal/cuentatiempo;
}

void updateLine(int ETQ, int linea, int BLOCK, unsigned char addr_bin[]){

	//Declaramos la variables

	int p = 0, addr_BLOCKbin[1024], addr_WORDbin[1024];

	//Convertimos todas las direcciones de RAM (bin) por Bloque y Palabra / Convert all addresses of RAM (bin) by Block and Word.

	for(int k = 0; k<1024; k++){
		addr_BLOCKbin[k] = (addr_bin[k] & bit_BLOCK) >> 3;
		addr_WORDbin[k] = (addr_bin[k] & bit_WORD);
	}
						
	proceso.tiempoglobal += 10;	
	cuentatiempo++;

	//Asignamos ese fallo a la ETQ de cache y empezaremos a comprobar en RAM(bin) si esta la ETQ / LABEL of cache takes the value from the LABELmemory and we look for that LABEL in RAM

	proceso.cache[linea].ETQ = ETQ;

	for(;p<8; p++){
		proceso.cache[linea].palabras[p] = addr_bin[BLOCK*8+p];
	}	
	
	//sleep(2);
}

void mostrarCache(){//Show cache

	for(int i=0; i < 4; i++){

		if(proceso.cache[i].ETQ < 9){
			printf("\nETQ: %X   DATOS: ", proceso.cache[i].ETQ);
		}else{
			printf("\nETQ: %X  DATOS: ", proceso.cache[i].ETQ);
		}
		
		for(int r = 7; r >= 0; r--){
			if(proceso.cache[i].palabras[r] != 0x0){
				printf("%X ", proceso.cache[i].palabras[r]);
			}else
			{
				printf("%d0 ", proceso.cache[i].palabras[r]);
			}
			
				
		}
		
		//sleep(0.8);
	}
	printf("\n\n");
	
	//sleep(1);
}

void contenidoCache(){//Cache content
	
	//sleep(0.5);

	printf("\n	CACHE	\n");
	for(int i=0; i < 4; i++){

		printf("\nContenido: ");

		for(int r = 7; r>=0; r--){

			printf("%c ", proceso.cache[i].palabras[r]);
			//sleep(1);
		}	
	}

	printf("\n\n-----------------------------------------------------------\n\n");
	//sleep(1);
}
