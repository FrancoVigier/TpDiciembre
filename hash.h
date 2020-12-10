#ifndef HASH_H_INCLUDED
#define HASH_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <wchar.h>
#include <math.h>

#include "LSE.h"

#define BUFFER_SIZE 1024
#define CAPACIDAD_INICIAL 1000


/**
* Estructura que representa el area de rebalse, un array de Listas
* Simplemente Enlazadas, para solucionar las colisiones en mi tabla Hash
*/
typedef struct EntradaHash_ {
  ListaEnlazada* listaEnlazada;
} EntradaHash;

/**
* Estructura que representa nuestra tabla Hash, la cual cuenta con la tabla
* con area de rebalse, la capacidad y el numero de elementos en la tabla
*/
typedef struct {
  EntradaHash* entradas;
  unsigned int capacidad;
  int numeroDeInformes;
} Notificaciones;

/**
* Funcion hash de nuestra tabla
*/
unsigned int hash(char* nombre);

/**
* Funcion de inicializacion de nuestra tabla Hash
*/
Notificaciones * notificaciones_crear();

/**
* Funcion de liberacion de lista enlazada, la cual va a liberar el area
* de rebalse de nuestra tabla, ademas cuenta con una bandera para liberar
* la memoria del contenido de cada nodo de la lista o solamente
* liberar los punteros a los nodos
*/
void destruir_entradas(EntradaHash* entradas, unsigned int capacidad,
                       bool eliminarNombres);

/**
* Funcion que elimina una entrada de nuestra tabla, la entrada se
* define a partir de la localidad, fecha y departamento en el
* formato descripto en el enunciado
*/
bool notificaciones_eliminar(Notificaciones* agenda, char* localidad,
                             char* fecha, char* departamento);

/**
* Funcion que busca una entrada de nuestra tabla, la entrada se
* define a partir de la localidad, fecha y departamento en el
* formato descripto en el enunciado
*/
void notificaciones_buscar(Notificaciones* agenda, char* localidad,
                           char* fecha, char* departamento);

/**
* Funcion que aumenta la capacidad de nuestra tabla Hash y agranda la tabla
*/
void aumentar_capacidad(Notificaciones* agenda);

/**
* Inserta un nodo en una lista simplemente enlazada, teniendo en
* cuenta el valor de la clave calculada con nuestra funcion hash
* tomando como variable la Localidad
*/
void insertar_entrada(EntradaHash* entradas, unsigned int capacidad,
                      char* fecha, char* departamento, char* localidad,
                      int confirmados, int descartados, int enEstudio,
                      int notificaciones);

/**
* Funcion que inserta una entrada a la tabla, la entrada se define a partir
* de localidad, fecha, departamento, casos confirmados, casos descartados,
* casos en estudio y notificaciones en el formato dado en el enunciado. En el
* caso de ya existir un valor para esa entrada la sobreescribe
*/
void notificaciones_insertar(Notificaciones* agenda, char* fecha,
                             char* departamento, char* localidad,
                             int confirmados, int descartados,
                             int enEstudio, int notificaciones);

/**
* Funcion que libera toda la memoria pedida por nuestra tabla
*/
void notificaciones_destruir(Notificaciones* agenda);

#endif // HASH_H_INCLUDED
