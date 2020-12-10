#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED
#include <stdio.h>
#include <stdlib.h>
#include "LSE.h"
#include "hash.h"
/**
 * Ba'haal the end of line eater
 * Come todo hasta la siguiente linea.
 * Si la siguiente linea contiene un EOF al inicio, tambien lo come.
 */
void comer_finales_de_linea(FILE * archivo);
/**
* Funcion que dada una linea del archivo .csv, extrae los valores
* de los parametros entre los separadores ","
*/
Diario* parsear_linea(char* linea);
/**
* Funcion que desglosa el contenido del archivo( pasando como
* argumento su nombre) y carga en la tabla hash todo el contenido del mismo
*/
void parser_cvs(char* nombreArchivo, Notificaciones* agenda);
/**
* Funcion que va a leer de la consola la entrada del usuario y
* la divide en dos partes. La primera que es la operacion y la
* segunda que son los argumentos
*/
void parsear_comando_y_operacion(char* comando, char* operacion);
/**
* Funcion que agarra la entrada de los argumentos y segun la operacion
* los almacenamos, ya que cada instruccion en el interprete recibe
* sus argumentos en un orden predefinido
*/
void parser_elim_acum_dup(char* operacion, char* fecha, char* localidad,
                          char* dpto, int* confirmados, int* descartados,
                          int* enEstudio, char* fechaInicio,char* fechaFinal,
                          char* nombreArchivo , int parser);
/**
* Funcion que asigna a cada instruccion un indice de ejecucion
*/
int comando_int(char* operacion, char* alias);
/**
* Funcion que vuelca el contenido de una tabla hash sobre un archivo .csv
*/
void crear_salida_cvs (char* nombreArchivo, Notificaciones* agenda);
#endif // PARSER_H_INCLUDED
