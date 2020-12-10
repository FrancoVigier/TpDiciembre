#include "hash.h"
#include "LSE.h"
#include <string.h>
#include <stdlib.h>

/**
* Funcion hash de nuestra tabla
*/
unsigned int hash(char* nombre) {
  const int constantePrima = 17;
  unsigned int hash = constantePrima;
  for(size_t i = 0; i < strlen(nombre); i++) {
    hash = hash * constantePrima + nombre[i];
  }
  return hash;
}

/**
* Funcion de inicializacion de nuestra tabla Hash
*/
Notificaciones* notificaciones_crear() {
  //Introducimos una variable global en cuanto a la capadidad inicial
  //de nuestra tabla
  int capacidad = CAPACIDAD_INICIAL;
  //Inicializamos nuestra area de rebalse
  EntradaHash* entradas = calloc(capacidad, sizeof(EntradaHash));
  //Creamos nuestra tabla
  Notificaciones* informe = malloc(sizeof(Notificaciones));
  //Cargamos el contenido inicial a nuestra tabla
  *informe = (Notificaciones) {.entradas = entradas,
                               .capacidad = capacidad,
                               .numeroDeInformes = 0};
  return informe;
}

/**
* Funcion de liberacion de lista enlazada, la cual va a liberar el area
* de rebalse de nuestra tabla, ademas cuenta con una bandera para
* liberar la memoria del contenido de cada nodo de la lista o
* solamente liberar los punteros a los nodos
*/
void destruir_entradas(EntradaHash* entradas, unsigned int capacidad,
                       bool eliminarNombres) {
  //Recorremos toda nuestra area de rebalse
  for (size_t indice = 0; indice < capacidad; indice++) {
    ListaEnlazada* lista = entradas[indice].listaEnlazada;
  //Recorremos cada LSE en nuestra area de rebalse
    while (lista != NULL) {
      ListaEnlazada* sig = lista->siguiente;
  //Si la bandera es verdadera, liberamos la memoria pedida en el contenido de
  //cada nodo
      if (eliminarNombres) {
        free(lista->informe.localidad);
        free(lista->informe.departamento);
        free(lista->informe.hora);
      }
      free(lista);
      lista = sig;
    }
  }
  //Liberamos el puntero a la tabla
  free(entradas);
}

/**
* Funcion que elimina una entrada de nuestra tabla, la entrada se define
* a partir de la localidad, fecha y departamento en el formato descripto
* en el enunciado
*/
bool notificaciones_eliminar(Notificaciones* agenda, char* localidad,
                             char* fecha, char* departamento) {
  //Calculo el indice de la localidad pasada como parametro
  unsigned int hashCalculado = hash(localidad);
  unsigned int indice = hashCalculado % agenda->capacidad;
  //Obtengo la LSE correspondiente a ese indice
  ListaEnlazada * lista = agenda->entradas[indice].listaEnlazada;
  ListaEnlazada * penultimoElem = NULL;
  //Recorro la lista y si el nodo a eliminar se encuentra, me
  //posiciono en un nodo anterior a él
  while (lista != NULL && (lista->clave != hashCalculado ||
        (strcmp(lista->informe.localidad, localidad) != 0 ||
         strcmp(lista->informe.hora, fecha) != 0 ||
         strcmp(lista->informe.departamento, departamento) != 0))) {
    penultimoElem = lista;//nos paramos detras de informe->localidad
    lista = lista->siguiente;
  }
  if (lista != NULL) {//Si esta en la agenda, lo elimino
    if (penultimoElem == NULL) {
      agenda->entradas[indice].listaEnlazada = NULL;
    } else {//Lista en su dato es el que tiene ese localidad
      penultimoElem->siguiente = lista->siguiente;
    }
    //Libero la memoria en los datos del nodo
    free(lista->informe.localidad);
    free(lista->informe.departamento);
    free(lista->informe.hora);
    free(lista);
    //Le resto al contador de informes en la Tabla
    agenda->numeroDeInformes--;
    return true;
  }
  return false;
}

/**
* Funcion que busca una entrada de nuestra tabla, la entrada se define a partir
* de la localidad, fecha y departamento en el formato descripto en el enunciado
*/
void notificaciones_buscar(Notificaciones* agenda, char* localidad, char* fecha,
                           char* departamento) {
  //Calculo el indice de la localidad pasada como parametro
  unsigned int hashCalculado = hash(localidad);
  unsigned int indice = hashCalculado % agenda->capacidad;
  //Obtengo la LSE correspondiente a ese indice
  ListaEnlazada * lista = agenda->entradas[indice].listaEnlazada;
  //Recorro la LSE hasta que encuentre un nodo con los mismos datos
  //que los pasados por parametro
  while (lista != NULL && (lista->clave != hashCalculado ||
        (strcmp(lista->informe.localidad, localidad) != 0 ||
         strcmp(lista->informe.hora, fecha) != 0 ||
         strcmp(lista->informe.departamento, departamento) != 0))) {
    lista = lista->siguiente;
  }
  //Si la LSE no es nula significa que se encontro un nodo
  //con los datos pedidos, y la LSE apunta a ese nodo
  if (lista != NULL) {
    printf("SE ENCONTRO: \n");
    printf("FECHA: %s\n", lista->informe.hora);
    printf("DEPARTAMENTO: %s\n", lista->informe.departamento);
    printf("LOCALIDAD: %s\n", lista->informe.localidad);
    printf("CONFIRMADOS: %i\n", lista->informe.confirmados);
    printf("DESCARTADOS: %i\n", lista->informe.descartados);
    printf("EN ESTUDIO: %i\n", lista->informe.enEstudio);
    printf("NOTIFICACIONES: %i\n", lista->informe.notificaciones);
    return;
  } else {
      printf("NO ESTA\n"); //Si la lista es nula, no esta ese nodo
      return;
  }
}

/**
* Funcion que aumenta la capacidad de nuestra tabla Hash y agranda la tabla
*/
void aumentar_capacidad(Notificaciones* agenda) {
  //Configuramos una nueva capacidad para la nueva tabla, esta es
  //la capacidad anterior mas la CAPACIDAD_INICIAL
  unsigned int capacidadNueva = agenda->capacidad + CAPACIDAD_INICIAL;
  //Creamos un array de LSE con la capacidad redefinida
  EntradaHash* entradasNuevas = calloc(capacidadNueva, sizeof(EntradaHash));
  for (size_t i = 0; i < agenda->capacidad; ++i) {
    ListaEnlazada* lista = agenda->entradas[i].listaEnlazada;
    while (lista != NULL) {
      lista = lista->siguiente;
    }
  }
  destruir_entradas(agenda->entradas, agenda->capacidad, false);
  agenda->entradas = entradasNuevas;
  agenda->capacidad = capacidadNueva;
}

/**
* Inserta un nodo en una lista simplemente enlazada, teniendo en
* cuenta el valor de la clave calculada con nuestra funcion hash
* tomando como variable la Localidad
*/
void insertar_entrada(EntradaHash* entradas, unsigned int capacidad,
                      char* fecha, char* departamento, char* localidad,
                      int confirmados,int descartados, int enEstudio,
                      int notificaciones) {
  //Calculo el indice de la localidad pasada como parametro
  unsigned int hashCalculado = hash(localidad);
  unsigned int indice = hashCalculado % capacidad;
  //Creo un nodo LSE
  ListaEnlazada* nuevoElemento = calloc(1, sizeof(ListaEnlazada));
  // Almacenamos los datos ingresados
  nuevoElemento->clave = hashCalculado;
  nuevoElemento->informe = (Diario) {.hora = fecha,
                                     .departamento = departamento,
                                     .localidad = localidad,
                                     .confirmados = confirmados,
                                     .descartados = descartados,
                                     .enEstudio = enEstudio,
                                     .notificaciones = notificaciones};
  //Conformo el apuntador de nuesto nodo,
  //para que inserte al comienzo de la LSE
  if (entradas[indice].listaEnlazada != NULL) {
    nuevoElemento->siguiente = entradas[indice].listaEnlazada;
  }
  //Igualo la LSE al nodo conformado que fue insertado al
  //comienzo de la misma
  entradas[indice].listaEnlazada = nuevoElemento;
}

/**
* Funcion que inserta una entrada a nuestra tabla, la entrada se define a partir
* de la localidad, fecha, departamento, casos confirmados, casos descartados,
* casos en estudio y notificaciones en el formato dado en el enunciado. En el
* caso de ya existir un valor para esa entrada la sobreescribe
*/
void notificaciones_insertar(Notificaciones * agenda, char* fecha,
                             char* departamento, char* localidad,
                             int confirmados, int descartados,
                             int enEstudio, int notificaciones) {
  //Como al insertar se tienen que pisar los valores viejos,
  //si es que estos existen, elimino el nodo con los valores
  //pasados como parametro( si es que existe). Asi tengo garantia
  //de que se pisa el valor viejo
  notificaciones_eliminar(agenda, localidad, fecha, departamento);
  //Como insertamos aumentamos el contador de informes en la tabla
  agenda->numeroDeInformes++;
  //Inicializo cadenas para guardar en nuestra tabla por copia
  char* fechaAGuardar = calloc(strlen(fecha) + 1, sizeof(char));
  strcpy(fechaAGuardar, fecha);

  char* departamentoAGuardar = calloc(strlen(departamento) + 1, sizeof(char));
  strcpy(departamentoAGuardar, departamento);

  char* localidadAGuardar = calloc(strlen(localidad) + 1, sizeof(char));
  strcpy(localidadAGuardar, localidad);

  int confirmadosAGuardar = confirmados;
  int descartadosAGuardar = descartados;
  int enEstudioAGuardar = enEstudio;
  int notificacionesAGuardar = notificaciones;
  //Insertamos en el array de LSE nuestras variables por copia
  insertar_entrada(agenda->entradas, agenda->capacidad, fechaAGuardar,
                   departamentoAGuardar, localidadAGuardar, confirmadosAGuardar,
                   descartadosAGuardar, enEstudioAGuardar,
                   notificacionesAGuardar);
}

/**
* Funcion que libera toda la memoria pedida por nuestra tabla
*/
void notificaciones_destruir(Notificaciones* agenda) {
  //Destruimos todas nuestras LSE del area de rebalse y
  //los datos almacenados en sus nodos( por eso la bandera es true)
  destruir_entradas(agenda->entradas, agenda->capacidad, true);
  //Liberamos el puntero a la tabla
  free(agenda);
}
