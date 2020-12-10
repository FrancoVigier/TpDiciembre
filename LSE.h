#ifndef LSE_H_INCLUDED
#define LSE_H_INCLUDED
/**
* Estructura que va a representar un informe diario de cierta localidad
* Sus campos son los descriptos en el enunciado del TP
*/
typedef struct Diario_{
  char* hora;
  char* departamento;
  char* localidad;
  int confirmados;
  int descartados;
  int enEstudio;
  int notificaciones;
} Diario;

/**
* Estructura que va a representar una fecha dd/mm/aa guardando como enteros
* los valores correspondiente al dia, mes y año
*/

typedef struct Fecha_ {
  int dia;
  int mes;
  int ano;
} Calendario;

/**
* Estructura de lista simplemente enlazada, con la variante de que le agregamos
* parámetros funcionales a nuestra estructura Hash, como lo es la clave
*/
typedef struct ListaEnlazada_ {
  unsigned int clave;
  Diario informe;
  struct ListaEnlazada_* siguiente;
} ListaEnlazada;

/**
* Funcion que intercambia la data de dos nodos de una lista simplemente
* enlazada
*/
void glist_intercambiar(ListaEnlazada* nodoA, ListaEnlazada* nodoB);

/**
* Funcion que devuelve un entero que nos permite saber si el segundo
* parametro es mayor o menor que el primero
*/
int funcion_comparacio(int primero, int segundo);

/**
* Funcion de ordenamiento de lista, Selection Sort
*/
ListaEnlazada* glist_selection_sort(ListaEnlazada* lista);

/**
* Funcion comparadora de fechas, en la cual nos devuelve si la fecha del
* primer argumento ocurrio antes que la fecha del segundo argumento
*/
int compare_dates(Calendario* fechaUno, Calendario* fechaDos);

/**
* Funcion que dado un entero X nos devuelve 10^X
*/
int potencia_de_diez_int(int largo);

/**
* Esta funcion transforma una cadena de numerica de texto, en el valor numerico
* que esta representa
*/
int numero_char_a_int(char* numero);

/**
* La funcion toma como primer parametro una cadena de texto que representa
* una fecha con el formato "aa-mm-dd" y devuelve una estructura cuyo
* contenido son los valores en enteros del año, mes y dia
*/
void fecha_a_numero (char* fecha, Calendario* aammdd);
#endif // LSE_H
