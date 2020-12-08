#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

// prototipos de funciones

FILE* runRAM(); 
FILE* runAddr(); 
void runCache();
void mostrarCache();
void blockCheck(FILE* memory, FILE* bin);
void updateLine(FILE* bin, int h, int memory);

typedef struct{ 		//estructura de la linea; tiene etiqueta y 8 palabras. / Struct LINE; contains a LABEL and 8 WORDS.

	short int ETQ;
	short int palabras[8];

}T_LINEA_CACHE;

typedef struct{			 //estructura del proceso; tiene tiempoglobal, numero de fallos y 4 lineas de cache. / Struct "Process"; has globaltime, nMiss, and 4 LINES from the Struct LINE.

	int tiempoglobal;
	int numerofallo;
	T_LINEA_CACHE cache[4];

}nPROCESS;

//Variables globales / Global Var

nPROCESS proceso;		 //Variable proceso; es una estructura y uno de sus elementos es otra estructura (T_LINEA_CACHE) / Global Process var;
unsigned char RAM[1024];	 //Aqui ira el contenido de 'RAM.bin' / Global array to save the data of file 'RAM.bin'
unsigned char memory_access[12]; //Aqui ira el contenido de 'accesos_memoria.txt' / Global array to save data of file 'accesos_memoria.txt'

//Mascaras de bits / bit-Mask

#define bit_BLOCK 0b1111111000 	 //Bloque / BLOCK
#define bit_ETQ 0b1111100000 	 //Etiqueta / LABEL
#define bit_LINE 0b0000011000	 //Linea / LINE
#define bit_WORD 0b0000000111 	 //Palabra / WORD


int main (){

	FILE* ram =	runRAM(); //Asignas el contenido de RAM.bin a la variable ram / Save the content of file 'RAM.bin' with func 'runRAM()'
	FILE* memory = runAddr(); //asignas el contenido de accesos_memoria.txt a la variable memory / Save the content form file 'accesos_memoria.txt' with func 'runAddr()' 

	runCache(); //Inicializas toda la cache con ETQ 0xFF y Datos (palabras) 0 / Initialize the cache with LABEL 0xFF and Data(WORDs) to 0 

	blockCheck(memory, ram);


	fclose(ram);
	fclose(memory);

	return 0;
}

//Funciones que inicializan RAM, Cache, Dirrecciones / Functions where initialize the Cache, RAM and Address sets.

void runCache(){

	const short int final_ETQ = 0xFF; //Valor inicial de las Etiquetas, FF (HEX) / Constant var LABEL with FF (HEX)

//	sleep(1);
	printf("Inicializando la Cache...\n");

//	sleep(3);

	//Bucle for para inicializar la linea / For-loop to initialize the cache line.
	for(int i=0; i < 4; i++){

		proceso.cache[i].ETQ = final_ETQ; //Asignas FF a todas las etiqueta / Initialize LABEL to FF with Constant var
		printf("La ETQ %X tiene un valor inicial de ", proceso.cache[i].ETQ);

		for(int j=0; j<8; j++){ //Asignas 0 a todas las palabras / Initialize every WORD to 0

			proceso.cache[i].palabras[j] = 0;
			printf("%d ", proceso.cache[i].palabras[j]);

		}
		printf("\n");
	}
}

FILE* runRAM(){ //Asignas el contenido de RAM.bin a la variable 'bin' / Pointer FILE* to 'RAM.bin' called 'bin'

	FILE* bin;

	bin = fopen("RAM.bin", "r");

	if(bin == NULL){ //Manejo de excepciones a la hora de abrir el fichero / Control-if the FILE doesn't exist return exit(-1)

		int H = 0;
		printf("\nEl fichero 'RAM.bin' no existe.\n\n");

		sleep(2);
		H=1;
		do{exit (-1);}while(H==1);

	}

	rewind(bin);
	return bin;
}


FILE* runAddr(){ //Asignas el contenido de 'accesos_memoria.txt' a la variable memory / Pointer FILE* to 'accesos_memoria.txt' called 'memory'

	// Abrimos el fichero / Open the FILE

	FILE *memory;
	memory = fopen("accesos_memoria.txt", "r");

	if(memory == NULL){ //Manejo de excepciones a la hora de abrir el fichero / Control-if the FILE doesn't exist return exit(-1)
		int H = 0;
		printf("\nEl fichero 'accesos_memoria.txt' no existe.\n\n");

		sleep(2);
		H=1;
		do{exit (-1);}while(H==1);
	}

	rewind(memory);

	return memory;
}

void blockCheck(FILE* memory, FILE* bin){

	//Declaramos e Inicializamos a 0 direccion, y la dirreccion de cada elemento dentro de la cache / Declare and initialize to 0 every address from cache.

	int addr_memory[12], addr_block[12], addr_ETQ[12], addr_line[12], addr_word[12], addr_bin[1024];

	for(int y=0 ; y<12 ; y++){
		addr_block[y] = 0;
		addr_ETQ[y] = 0;
		addr_line[y] = 0;
		addr_word[y] = 0;

	}

	//Escaneamos cada linea de las direcciones del fichero / Scan every address from the FILE.

	for(int y=0; y<12; y++){
		fscanf(memory, "%X", &addr_memory[y]);
	}

	//Comparamos con el operador logico AND si se corresponde a la mascara de Bits / Compare the addr with the logic operator AND using the mask bit of every element.

	for(int z = 0; z<12; z++){
		addr_block[z] = ( addr_memory[z] & bit_BLOCK ) >> 3; //Analizar el bloque de cada direccion y despues compararla con RAM.
		addr_ETQ[z] = ( addr_memory[z] & bit_ETQ ) >> 3;
		addr_line[z] = ( addr_memory[z] & bit_LINE ) >> 3;
		addr_word[z] = ( addr_memory[z] & bit_WORD );

		//Debug
		//printf("addr %#04x \n", addr_memory[z]);
	}

	printf("\nEscaneando los bloques..\n");
	//sleep(2);

	proceso.tiempoglobal = 0;
	proceso.numerofallo = 0;

	//Donde se miraran todas las direcciones con cada una de las lineas, aconsejo probar al reves a ver si printea mejor!!

	for(int r = 0; r<4; r++){

		for(int f = 0; f<12 ; f++){


			if(proceso.cache[r].ETQ != addr_ETQ[f]){
				printf("linea %d acceso de memoria %04X \n", f, addr_memory[f]);
				//sleep(2);

				//printf("T: %d | Fallo de CACHE %d | ADDR %04X | ETQ %X | linea %02X | palabra %02X | bloque %02X |\n", proceso.tiempoglobal, proceso.numerofallo, addr_memory[f], addr_ETQ[f], addr_line[f], addr_word[f], addr_block[f]);
				updateLine(bin, f, addr_memory[r]);
				sleep(1);
			}else{
				proceso.tiempoglobal++;
				printf("linea %d acceso de memoria %04X \n", f, addr_memory[f]);

				//printf("T: %d | Acierto de CACHE | ADDR %04X | ETQ %X | linea %02X | palabra %02X | DATO %02X|\n", proceso.tiempoglobal, addr_memory[f], addr_ETQ[f], addr_line[f], addr_word[f], proceso.cache[addr_line[f]].palabras[addr_word[f]]);
				//mostrarCache();
				sleep(1);

			}
		}
	}
}

void updateLine(FILE* bin, int h, int memory){

	//Declaramos la variables

	int t = 0, p = 0, comprobar = 0, addr_bin[1024], addr_BLOCKbin[1024], addr_ETQbin[1024], addr_WORDbin[1024], addr_BLOCKmemory, addr_ETQmemory, addr_LINEmemory, addr_WORDmemory;

	for(int b=0; b<1024; b++){
		fscanf(bin, "%X", &addr_bin[b]);
		//printf("%X\n", addr_bin[b]);
	}


	//Convertimos todos los *accesos de memoria* (memory) por Bloque y ETQ | Convertimos todas las direcciones de RAM (bin) por Bloque y Palabra /

	addr_BLOCKmemory = (memory & bit_BLOCK) >> 3;
	addr_ETQmemory = (memory & bit_ETQ) >> 3;
	addr_LINEmemory = (memory & bit_LINE) >> 3;
	addr_WORDmemory = (memory & bit_WORD);

	//Debug
	//printf("BLOCKm %X | ETQm %X | LINEm %X | WORDm %X \n", addr_BLOCKmemory, addr_ETQmemory, addr_LINEmemory, addr_WORDmemory);


	for(int k = 0; k<1024; k++){
		addr_ETQbin[k] = (addr_bin[k] & bit_ETQ) >> 3;
		addr_BLOCKbin[k] = (addr_bin[k] & bit_BLOCK) >> 3;
		addr_WORDbin[k] = (addr_bin[k] & bit_WORD);
		//Debug
		//printf("ETQ %X | BLOCK %X | WORD %X \n", addr_ETQbin[k], addr_BLOCKbin[k], addr_WORDbin[k]);
	}

	proceso.numerofallo ++;		//Sumamos uno al fallo y aumentamos el tiempo en 10 / Adding to nMISS 1, and globaltime 10
	proceso.tiempoglobal += 10;

	//Asignamos ese fallo a la ETQ de cache y empezaremos a comprobar en RAM(bin) si esta la ETQ / LABEL of cache takes the value from the LABELmemory and we look for that LABEL in RAM

	proceso.cache[h].ETQ = addr_ETQmemory;

	//Debug
	//printf("linea %d con ETQ %X", h, addr_ETQmemory);

	for(int u=0; u<1024; u++){

		if(addr_BLOCKmemory != addr_BLOCKbin[u]){

			//Debug
			//printf(" mem %02x  & bin %02x\n", addr_BLOCKmemory, addr_BLOCKbin[u]);

		}else{

			//AQUI TOCA HACER COMPROBACIONES DE ETIQUETAS Y PROBAR SI SALE UN MEJOR RESULTADO DESPUES EN CHECKBLOCK AL MOSTRAR LA CACHE.   addr_ETQmemory con addr_ETQbin[u]

			printf("ETQ iguales bin: %x  mem: %x\n", addr_ETQbin[u], addr_ETQmemory);
			printf("valor de la p = %d\n", u);

			//BUCLE QUE ESCRIBE LOS DATOS(PALABRAS) DE MENOR A MAYOR PESO bits (escribe en el array de manera invertida)
			for(;t<8; t++){
				for(p=u; p > (u-8); p--){

					printf("el valor de p %d, hasta %d, el array direcciona a %d\n", p, (u+8), (p+8));
					printf("valor de la linea h %d\n",h);

					proceso.cache[h].palabras[t] = addr_WORDbin[p+8];
					
					printf("PRINT ADDR (%04X) ETQ: %X | LINEA %d | PALABRA %X | Valor de P %d\n", memory, addr_ETQbin[u], h, proceso.cache[h].palabras[t], p );
					
					t++;
					
				}
				break; // ni preguntes..
			}
		}	
	}

	//mostrarCache();
}

void mostrarCache(){

	
	for(int i=0; i < 4; i++){
		printf("\nLa ETQ: %X   DATOS: ", proceso.cache[i].ETQ);
		for(int r = 0; r < 8; r++){

			printf("%X ", proceso.cache[i].palabras[r]);
		}
	}
	printf("\n\n");
}


	// if(comprobar = 1){
	// 	printf("T: %d, Acierto de CACHE, ADDR %04X ETQ %X linea %02X palabra %02X DATO %02X\n", proceso.tiempoglobal, addr_memory, addr_ETQmemory, addr_LINEmemory, proceso.cache[h].palabras[addr_LINEmemory]);
	// }else{
	// 	printf("T: %d, Fallo de CACHE %d, ADDR %04X ETQ %X linea %02X palabra %02X bloque %02X\n", proceso.tiempoglobal, proceso.numerofallo, addr_memory, addr_ETQmemory, addr_LINEmemory, addr_WORDmemory, addr_BLOCKmemory);
	// }
