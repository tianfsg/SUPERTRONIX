Realizado por Sebastián Fco. Gutiérrez Rojas && Jorge Martínez Hurtado

Este programa en C simula el sistema de ficheros de un sistema operativo Linux, utilizando una estructura de proceso y una estructura de línea de cache. El proceso tiene una variable global que contiene información sobre el tiempo global, el tiempo medio, el número de fallos y el número de accesos, así como una matriz de cuatro líneas de cache. La estructura de línea de cache tiene dos variables: una etiqueta y ocho palabras.

El programa utiliza varias funciones para inicializar la RAM, la cache y las direcciones. La función "runRAM" se encarga de asignar el contenido del archivo "RAM.bin" a una matriz global de RAM. La función "runAddr" se encarga de asignar el contenido del archivo "accesos_memoria.txt" a una variable FILE. La función "blockCheck" utiliza esta variable FILE para verificar cada bloque de la memoria RAM y actualizar la línea de la cache en consecuencia.

El programa también tiene funciones para mostrar el contenido de la cache en caracteres y para actualizar una línea de la cache dada una dirección de etiqueta de memoria, una línea y una dirección de bloque de memoria.

Finalmente, el programa imprime el número de accesos, el número total de fallos, el tiempo total y el tiempo medio, y termina con una pausa antes de finalizar.

Es importante mencionar que el esqueleto del código fue desarrollado por el profesor Francisco Javier Algarra.
