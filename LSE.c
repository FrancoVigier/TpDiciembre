#include "hash.h"
#include "LSE.h"
#include <string.h>
#include <stdlib.h>

/**
* Funcion comparadora de fechas, en la cual nos devuelve si la fecha del
* primer argumento ocurrio antes que la fecha del segundo argumento
*/
int compare_dates(Calendario* fechaUno, Calendario* fechaDos) {
  //Comparamos las fechas respetando, primero comparando los años
  //de caso negativo comparamos los meses y por ultimo los años
  if (fechaUno->ano < fechaDos->ano)
    return 0;
  else if (fechaUno->ano > fechaDos->ano)
    return 1;
  if (fechaUno->ano == fechaDos->ano) {
    if (fechaUno->mes < fechaDos->mes)
      return 0;
    else if (fechaUno->mes > fechaDos->mes)
      return 1;
      else if (fechaUno->dia < fechaDos->dia)
        return 0;
        else if (fechaUno->dia > fechaDos->dia)
          return 1;
          else
            return 0;
    }
    return 0;
}

/**
* Funcion que dado un entero X nos devuelve 10^X
*/
int potencia_de_diez_int(int largo) {
  int potencia = 1;
  while (largo > 0) {
  //Acarreamos el valor de la potencia de 10
    potencia = 10 * potencia;
    largo--;
  }
  return potencia;
}

/**
* Esta funcion transforma una cadena de numerica de texto, en el valor numerico
* que esta representa
*/
int numero_char_a_int(char* numero) {
  int indice = 0;
  int largo = strlen(numero);
  int cifra = 0;
  int decimal;
  int negativo = 0;
  //Si la cadena es de un numero negativo el '-' va al comienzo
  if (numero[0] == '-') {
    negativo = 1;
    indice = 1;
    largo--;
  }
  //A cada caracter en la cadena lo paso a digito y lo multiplico
  //por su potencia de 10 correspondiente a su posicion
  while (numero[indice] != '\0') {
    decimal = numero[indice] - '0';
    cifra += decimal * potencia_de_diez_int(--largo);
    indice++;
  }
  //Si es negativo multiplico el resultado por 1
  if (negativo == 1) {
    cifra = cifra * -1;
  }
  return cifra;
}

/**
* La funcion toma como primer parametro una cadena de texto que representa
* una fecha con el formato "aa-mm-dd" y devuelve una estructura cuyo
* contenido son los valores en enteros del año, mes y dia
*/
void fecha_a_numero (char* fecha, Calendario* aammdd) {
  char* buffer =  calloc(27 + 1, sizeof(char));
  strcpy(buffer, fecha);
  buffer[10] = '-';
  char* fraccion = strtok(buffer, "-");
  int parseo = 0;
  //Pido memoria para guardar las cadenas que representan
  //al año, mes y dia
  char* ano = calloc(4 + 1, sizeof(char));
  char* mes = calloc(2 + 1, sizeof(char));
  char* dia = calloc(2 + 1, sizeof(char));
  //Separo la cadena original, y sabiendo el preorden de las cadenas
  //puedo guardarlas facilmente
  if (fraccion != NULL) {
    while (fraccion != NULL) {
      if(parseo == 0)
        strcpy(ano, fraccion);
      if(parseo == 1)
        strcpy(mes, fraccion);
      if(parseo == 2)
        strcpy(dia, fraccion);
      parseo++;
      fraccion = strtok(NULL, "-");
    }
  }
  //Guardo en la estructura parametro el valor numerico de las
  //cadenas recien obtenidas
  aammdd->ano = numero_char_a_int(ano);
  aammdd->mes = numero_char_a_int(mes);
  aammdd->dia = numero_char_a_int(dia);
  //Libero la memoria pedida
  free(ano);
  free(mes);
  free(dia);
  free(buffer);
}

/**
* Funcion que intercambia la data de dos nodos de una lista simplemente
* enlazada
*/
void glist_intercambiar(ListaEnlazada* nodoA, ListaEnlazada* nodoB) {
  //Creo una estructura Diario que es la data de nuestra LSE y tambien
  //guardo la clave de esta
  Diario nodoC = nodoB->informe;
  int claveC = nodoB->clave;
  //Cambio los datos de los nodos
  nodoB->informe = nodoA->informe;
  nodoB->clave = nodoA->clave;
  nodoA->informe = nodoC;
  nodoA->clave = claveC;
}

/**
* Funcion que devuelve un entero que nos permite saber si el segundo
* parametro es mayor o menor que el primero
*/
int funcion_comparacio(int primero, int segundo) {
  return primero < segundo;
}

/**
* Funcion de ordenamiento de lista, Selection Sort
*/
ListaEnlazada* glist_selection_sort(ListaEnlazada* lista) {
  ListaEnlazada* nodo = lista;
  Calendario* fechaNodo2  = malloc(sizeof(struct Fecha_));
  Calendario* fechaAux = malloc(sizeof(struct Fecha_));
  for (;nodo->siguiente != NULL; nodo = nodo->siguiente) {
  //Guardo en aux el nodo que tenga el dato menor segun la comparacion.
    ListaEnlazada* aux = nodo;
    fecha_a_numero(aux->informe.hora,fechaAux);
  //Itero sobre el siguiente elemento de la lista
    ListaEnlazada* nodo2 = nodo->siguiente;
    for (;nodo2 != NULL; nodo2 = nodo2->siguiente) {
     fecha_a_numero(nodo2->informe.hora,fechaNodo2);
  //Si el nodo sobre el que se itera es menor al guardado, lo guardo.
     if (compare_dates(fechaNodo2, fechaAux) == 0) {
        aux = nodo2;
      }
    }
  //Si hubo cambios en el nodo auxiliar, entonces intercambio los valores.
    if (aux != nodo) {
      glist_intercambiar(nodo, aux);
    }
  }
  //Libero la memoria usada para almacenar la conversion de fechas
  free(fechaNodo2);
  free(fechaAux);
  return lista;
}
