#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


FILE* runRAM();
FILE* runAddr();
void runCache();
int mostrarCache(/*int i*/);
void blockCheck(FILE* bin, FILE* memory);
void updateLine(int r, FILE* bin, int memory);

// Estructura de linea y del proceso por defecto / Struct line of cache and the default proccess

typedef struct{
	short int ETQ;
	short int palabras[8];

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
	
	runCache();

	FILE* bin = runRAM();
	FILE* memory = runAddr();

	blockCheck(bin, memory);


	fclose(memory);
	fclose(bin);
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
			proceso.cache[i].palabras[j] = 0;
			printf("%d ", proceso.cache[i].palabras[j]);
		}
		printf("\n");
	}

}

FILE* runRAM(){

	FILE* bin;

	bin = fopen("RAM.bin", "r");

	if(bin == NULL){
		int H = 0;
		printf("\nEl fichero 'RAM.bin' no existe.\n\n");

		sleep(2);
		H=1;
		do{exit (-1);}while(H==1);
	}

	rewind(bin);

	return bin;
}


FILE* runAddr(){

	// Abrimos el fichero / Open the FILE

	FILE *memory;
	memory = fopen("accesos_memoria.txt", "r");

	//Si el fichero no existe 'segmention fault error' / If the file doesn't exist it breaks 
	if(memory == NULL){
		int H = 0;
		printf("\nEl fichero 'accesos_memoria.txt' no existe.\n\n");

		sleep(2);
		H=1;
		do{exit (-1);}while(H==1);
	}

	rewind(memory);

	return memory;
}

void blockCheck(FILE *bin, FILE* memory){
//Declaramos e Inicializamos a 0 direccion, y la dirreccion de cada elemento dentro de la cache / Declare and initialize to 0 every address from cache.

	int addr_memory[12], addr_block[12], addr_ETQ[12], addr_line[12], addr_word[12], addr_bin[1023];

	for(int y=0 ; y<12 ; y++){

		addr_memory[y] = 0;
		addr_block[y] = 0;
		addr_ETQ[y] = 0;
		addr_line[y] = 0;
		addr_word[y] = 0;

	}

//Escaneamos cada linea de las direcciones del fichero / Scan every address from the file.

	for(int z = 0; z<12; z++){
		fscanf(memory, "%X", &addr_memory[z]);

		addr_block[z] = ( addr_memory[z] & bit_BLOCK ) >> 3; //Analizar el bloque de cada direccion y despues compararla con RAM.
		addr_ETQ[z] = ( addr_memory[z] & bit_ETQ ) >> 3;
		addr_line[z] = ( addr_memory[z] & bit_LINE ) >> 3;
		addr_word[z] = ( addr_memory[z] & bit_WORD );


	//Debug
	//	printf("addr %#04x \n", addr[z]);
	}

    for(int j = 0; j< 1024; j++){

        fscanf(bin, "%X", &addr_bin[j]);
    }

//Comparamos con el operador logico AND si se corresponde a la mascara de Bits / Compare the addr with the logic operator AND using the mask bit of every element.


	printf("\nEscaneando los bloques..\n");
	//sleep(2);

	proceso.tiempoglobal = 0;
	proceso.numerofallo = 0;

	for(int r = 0; r<4; r++){
		for(int f = 0; f<12 ; f++){


			if(proceso.cache[r].ETQ == addr_ETQ[f]){
				proceso.tiempoglobal++;
				printf("T: %d | Acierto de CACHE | ADDR %04X | ETQ %X | linea %02X | palabra %02X | DATO %02X|\n", proceso.tiempoglobal, addr_memory[f], addr_ETQ[f], addr_line[f], addr_word[f], proceso.cache[r].palabras[addr_line[f]]);
				//mostrarCache(r);
			}else{
				printf("T: %d | Fallo de CACHE %d | ADDR %04X | ETQ %X | linea %02X | palabra %02X | bloque %02X |\n", proceso.tiempoglobal, proceso.numerofallo, addr_memory[f], addr_ETQ[f], addr_line[f], addr_word[f], addr_block[f]);
				updateLine(r, bin, addr_memory[f]);
				//mostrarCache(r);
			}
		}
		printf("\n");
	}
	
}

void updateLine(int h, FILE* bin, int addr_memory){
	//Declaramos la variables

	int t = 0, p = 0, comprobar = 0, addr_bin[1023], addr_BLOCKbin[1023], addr_ETQbin[1023], addr_WORDbin[1023], addr_BLOCKmemory, addr_ETQmemory, addr_LINEmemory, addr_WORDmemory;

	//Convertimos todos los *accesos de memoria* (memory) por Bloque y ETQ | Convertimos todas las direcciones de RAM (bin) por Bloque y Palabra /

	addr_BLOCKmemory = (addr_memory & bit_BLOCK) >> 3;
	addr_ETQmemory = (addr_memory & bit_ETQ) >> 3;
	addr_LINEmemory = (addr_memory & bit_LINE) >> 3;
	addr_WORDmemory = (addr_memory & bit_WORD);

	do{
		fscanf(bin, "%X", &addr_bin[t]);
		t++;
	}while(t < 1024);

	for(int k = 0; k<1024; k++){
		addr_ETQbin[k] = (addr_bin[k] & bit_ETQ) >> 3;
		addr_BLOCKbin[k] = (addr_bin[k] & bit_BLOCK) >> 3;
		addr_WORDbin[k] = (addr_bin[k] & bit_WORD);
	}

	

	proceso.numerofallo ++;
	proceso.tiempoglobal += 10;

	//asignamos ese fallo a la ETQ de cache y empezaremos a comprobar en RAM(bin) si esta /
	proceso.cache[h].ETQ = addr_ETQmemory;
	
	for(int u =0; u< 1024; u++){
		if(addr_BLOCKbin[u] == addr_BLOCKmemory){
			for(int j=0; j < 9 ; j++){
				for(int x = u+9; x > x-9; x--){
					proceso.cache[h].palabras[j] = addr_WORDbin[x];
				}
			}
		}
		
	}

	// if(comprobar = 1){
	// 	printf("T: %d, Acierto de CACHE, ADDR %04X ETQ %X linea %02X palabra %02X DATO %02X\n", proceso.tiempoglobal, addr_memory, addr_ETQmemory, addr_LINEmemory, proceso.cache[h].palabras[addr_LINEmemory]);
	// }else{
	// 	printf("T: %d, Fallo de CACHE %d, ADDR %04X ETQ %X linea %02X palabra %02X bloque %02X\n", proceso.tiempoglobal, proceso.numerofallo, addr_memory, addr_ETQmemory, addr_LINEmemory, addr_WORDmemory, addr_BLOCKmemory);
	// }

	
}

int mostrarCache(){

	
	for(int i=0; i = 4; i++){
		printf("La ETQ %X tiene un valor de ", proceso.cache[i].ETQ);
		for(int r = 0; r < 9; r++){

			printf("%d ", proceso.cache[i].palabras[r]);
		}
		printf("\n");
	}

	return 0;
}