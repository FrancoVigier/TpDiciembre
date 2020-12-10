#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <wchar.h>
#include <math.h>
#include <ctype.h>
#include "hash.h"
#include "LSE.h"
#define BUFFER_SIZE 1024
#define CAPACIDAD_INICIAL 1000
#define LIMITE 250

/**
 * Ba'haal the end of line eater
 * Come todo hasta la siguiente linea.
 * Si la siguiente linea contiene un EOF al inicio, tambien lo come.
 */
void comer_finales_de_linea(FILE * archivo) {
  int car;
  while ((car = fgetc(archivo)) == '\r' || car == '\n')
    if (car == '\r')
      fgetc(archivo);// come el \n
  if (car != EOF)
    ungetc(car, archivo);
}
/**
* Funcion que dada una linea del archivo .csv, extrae los valores
* de los parametros entre los separadores ","
*/
Diario* parsear_linea(char* linea) {
  //El ultimo parametro no posee el separador ',' entonces lo agrego a la linea
  linea[strlen(linea)] = ',';
  //Hago un primer fraccionamiento de la linea, para leer lo que hay en ella
  //pero entre comas
  char* fraccion = strtok(linea, ",");
  //Variable que lleva en cuenta el lugar predefinido de los datos en una linea
  //del archivo
  int parseo = 0;
  //Variables donde van a ir almacenados los datos extraidos
  char* fecha;
  char* departamento;
  char* localidad;
  int confirmados;
  int descartados;
  int enEstudio;
  int notificaciones;
  if (fraccion != NULL) {
  //Recorro el contenido de mi linea entre comas y lo guardo como variables
    while (fraccion != NULL) {
      parseo++;
      if (parseo == 1) {
        fecha=fraccion;
        printf("%s-",fecha);
      }
      if (parseo == 2) {
        departamento= fraccion;
        printf("%s-",departamento);
      }
      if (parseo == 3) {
        localidad = fraccion;
        printf("%s-",localidad);
      }
      if (parseo == 4) {
        confirmados = numero_char_a_int(fraccion);
        printf("%i-",confirmados);
      }
      if (parseo == 5) {
        descartados = numero_char_a_int(fraccion);
        printf("%i-",descartados);
      }
      if (parseo == 6) {
        enEstudio = numero_char_a_int(fraccion);
        printf("%i-",enEstudio);
      }
      if (parseo == 7) {
        notificaciones = numero_char_a_int(fraccion);
        printf("%i\n",notificaciones);
        parseo = 0;
      }
      fraccion = strtok(NULL, ",");
    }
  }
  //Almaceno todos los datos extraidos en una estructura
  Diario* dia = malloc(sizeof(struct Diario_));
  dia->hora = fecha;
  dia->localidad= localidad;
  dia->departamento= departamento;
  dia->confirmados = confirmados;
  dia->descartados = descartados;
  dia->enEstudio = enEstudio;
  dia->notificaciones = notificaciones;
  return dia;
}

/**
* Funcion que devuelve los casos acumulados confirmados de una localidad
* en una fecha en especifico
*/
int casos_acumulados(Notificaciones * agenda, char* localidad,
                     char* fecha, char* departamento) {
  //Calculo el indice de la LSE por medio de la funcion hash
  unsigned int hashCalculado = hash(localidad);
  unsigned int indice = hashCalculado % agenda->capacidad;
  char* fecharAMostrar = calloc(27 + 1, sizeof(char));
  int confirmadosAMostrar = 0;
  //Recorro la LSE en busqueda del nodo para extraerle su variable "confirmados"
  ListaEnlazada* lista = agenda->entradas[indice].listaEnlazada;
  for (;lista != NULL; lista = lista->siguiente) {
  //Si encuentra el nodo, guardo la fecha y la cantidad de confirmados
    if (strcmp(lista->informe.localidad,localidad) == 0 &&
        strcmp(lista->informe.hora, fecha) == 0 &&
        strcmp(lista->informe.departamento,departamento) == 0) {
      strcpy(fecharAMostrar, lista->informe.hora);
      confirmadosAMostrar = lista->informe.confirmados;
    }
  }
  free(fecharAMostrar);
  return confirmadosAMostrar;
}

/**
* Funcion que retorna el maximo valor de contagios diarios, para
* una localidad
*/
void buscar_pico (Notificaciones* agenda, char* localidad, char* departamento) {
  //Calculo el indice de la LSE por medio de la funcion hash
  unsigned int hashCalculado = hash(localidad);
  unsigned int indice = hashCalculado % agenda->capacidad;
  ListaEnlazada* lista = agenda->entradas[indice].listaEnlazada;
  int picoMaximo = 0;
  char* fechaPico = calloc(27 + 1, sizeof(char));
  //Ordeno la lista por el parametro de fechas de mas antigua a mas reciente
  //esto para poder facilitar el calculo de contagios diarios que es
  //cason confirmados de hoy menos los casos confirmados de ayer
  ListaEnlazada* nuevaLista = glist_selection_sort(lista);
  //Variable que almacenara el nodo del dia anterior para esa localidad
  ListaEnlazada* anterior = NULL;
  //Recorro la LSE
  for (;nuevaLista != NULL;nuevaLista = nuevaLista->siguiente) {
  //Si se trata de un nodo de esa localidad y departamento...
    if (strcmp(nuevaLista->informe.localidad, localidad) == 0 &&
        strcmp(nuevaLista->informe.departamento,departamento) == 0) {
  //Si el anterior en nulo significa que es el primer nodo en aparecer en la
  //lista ordenada, por lo cual el pico maximo (por el momento) es su variable
  //de casos confirmados
      if (anterior == NULL) {
        picoMaximo = nuevaLista->informe.confirmados;
        strcpy(fechaPico,nuevaLista->informe.hora);
        anterior = nuevaLista;
      } else {
  //Si el nodo anterior para esa localidad no es nulo, entonces calculamos
  //los casos diarios y si es mayor al pico que acarreamos lo pisamos
          if (nuevaLista->informe.confirmados - anterior->informe.confirmados
              > picoMaximo) {
  //Creo estas variables enteras porque el nombre excede los 80 carac
            int nuevaListaConfirm = nuevaLista->informe.confirmados;
            int anteriorConfirm = anterior->informe.confirmados;
            picoMaximo = nuevaListaConfirm - anteriorConfirm;
            strcpy(fechaPico,nuevaLista->informe.hora);
          }
  //Actualizo el puntero
          anterior = nuevaLista;
        }
    }
  }
  printf("El pico de contagios para la localidad de %s ",localidad);
  printf("en el dpto de %s es de %i ", departamento, picoMaximo);
  printf("en la fecha %s\n", fechaPico);
  free(fechaPico);
}
/**
* Funcion que dandole una lugar y una fecha, extrae los casos confirmados
* para ese lugar y calcula retrocediendo en el tiempo cuantos dias tardo
* en llegar a los confirmado encontrando un nodo anterior a él con un
* valor de confirmados de la mitad. En mi implementacion hay variantes,
* explicadas en el informe. Por ejemplo
* Valor de contagios
* 100 120 140 200 -> dias de duplicacion son 3, partiendo que nuestro
* dia parametro tiene 200 contagios
* 70 80 90 200 -> dias de duplicacion son 1, puesto que hubo un pico
* que contiene la mitad de los contagios de nuestra variable(90 a 200 en un dia)
* 200 -> dias de duplicacion son 0, ya que es el unico valor de contagios
*/
void tiempo_duplicacion(Notificaciones* agenda, char* fecha, char* localidad,
                         char* departamento) {
  //Extraigo los casos confirmados para la localidad
  int contagiosDuplicados = casos_acumulados(agenda, localidad, fecha,
                                             departamento);
  //Calculo la mitad de los casos confirmados, para saber a que numero
  //de contagio atras en el tiempo debo llegar(en el caso de que este
  // en la tabla)
  contagiosDuplicados = contagiosDuplicados / 2;
  printf("Casos acumulados: %i\n",casos_acumulados(agenda, localidad,
                                                   fecha, departamento));
  printf("Contagios duplicados: %i\n",contagiosDuplicados);
  //Calculo el indice mediante la funcion hash
  unsigned int hashCalculado = hash(localidad);
  unsigned int indice = hashCalculado % agenda->capacidad;

  ListaEnlazada * lista = agenda->entradas[indice].listaEnlazada;
  //Variable que lleva los dias de distancia
  int contadorDuplicacion = 1;
  //Variable que nos dice si es el unico nodo en la LSE de esa localidad
  int unico = 0;
  //Como tengo que ir atras en el tiempo me parece primordial ordenar
  //la lista de mas antiguo a mas reciente en cuanto a fechas
  ListaEnlazada* nuevaLista = glist_selection_sort(lista);
  printf("LA LISTA PARA LA LOCALIDAD %s ES:\n", localidad);
  //Recorro la LSE
  for(;nuevaLista != NULL;nuevaLista = nuevaLista->siguiente){
  //Voy a contar aquella fechas anteriores con casos de contagio mayores
  //a la mitad que los contagios de mi parametro
    if (strcmp(nuevaLista->informe.departamento, departamento) == 0 &&
        strcmp(nuevaLista->informe.localidad, localidad) == 0 &&
        nuevaLista->informe.confirmados > contagiosDuplicados) {
      unico++;
  //Si encuentro la fecha de mi parametro salgo del ciclo
      if (strcmp(nuevaLista->informe.hora, fecha) == 0) {
        break;
      }
  //Incremento en 1 los dias de duplicacion
      contadorDuplicacion++;
    }
  }
  //Si la lista es nula, significa que nunca encontros la fecha parametro
  //o que junto con localidad como el departamento no se encontraron
  if (nuevaLista == NULL) {
      printf("Fecha, localidad o departamento no encontrado\n");
    }
  //Si la lista no es nula y solo hizo 1 iteracion, entonces solo
  //para esa localidad hay un solo registro
  if (nuevaLista != NULL && unico == 1) {
    contadorDuplicacion = 0;
  }
  //Sino estamos en presencia de un pico 5, 100 o el caso 50, 60, 70, 100
  printf("El tiempo de duplicacion para la fecha %s,", fecha);
  printf(" en la localidad de %s del dpto %s es de %i\n", localidad,
         departamento, contadorDuplicacion);
}

/**
* Funcion que dado un intervalo de fechas, almacena en un archivo
* los casos diarios en todo ese intervalo
*/
void casos_diarios_intervalo(Notificaciones* agenda, char* localidad,
                             char* departamento, char* fechaInicio,
                             char* fechaFin) {
  //Abro el archivo a escribir
  FILE* fp = fopen("DiariosAGraficar.dat", "w");
  //Calculo el indice mediante la funcion hash
  unsigned int hashCalculado = hash(localidad);
  unsigned int indice = hashCalculado % agenda->capacidad;

  ListaEnlazada * nuevaLista = agenda->entradas[indice].listaEnlazada;
  //Con el mismo principio de buscar_pico, ordeno la lista
  ListaEnlazada* lista = glist_selection_sort(nuevaLista);
  //Puntero al anterior nodo de la misma localidad
  ListaEnlazada* anterior = NULL;
  //Variables donde se va a guardar las dos fechas extremos del intervalo
  //y la fecha del nodo en el momento de recorrer la LSE
  Calendario* fechaA = malloc(sizeof(struct Fecha_));
  Calendario* fechaB = malloc(sizeof(struct Fecha_));
  Calendario* fechaC = malloc(sizeof(struct Fecha_));
  fecha_a_numero(fechaInicio,fechaA);
  fecha_a_numero(fechaFin,fechaC);
  int indiceArchivo = 0;
  //Recorro la LSE
  for (;lista != NULL; lista = lista->siguiente) {
    if (strcmp(lista->informe.localidad,localidad) == 0 &&
        strcmp(lista->informe.departamento,departamento) == 0) {
  //Sea un nodo de esa localidad transformo su fecha y veo si
  //esta en el intervalo
      fecha_a_numero(lista->informe.hora,fechaB);
      if ((strcmp(lista->informe.hora, fechaInicio) == 0 &&
           strcmp(lista->informe.hora, fechaFin) == 0) ||
          (compare_dates(fechaB,fechaA) == 1 &&
          (compare_dates(fechaC,fechaB) == 1 ||
           strcmp(fechaFin,lista->informe.hora) == 0))) {
  //Si el anterior en nulo significa que es el primer nodo en
  //aparecer en la lista ordenada (en el intervalo dado),
  //por lo cual los casos diarios de ese dia es su variable de casos confirmados
        if (anterior == NULL) {
          printf("casos diarios en el dia %s en la localidad de %s es de %i\n",
                 lista->informe.hora,lista->informe.localidad,
                 lista->informe.confirmados);
          fprintf(fp, "%i %i\n", indiceArchivo++,lista->informe.confirmados);
          anterior = lista;
        } else {
  //Si el nodo anterior para esa localidad no es nulo, entonces calculamos
  //los casos diarios( esta en el intervalo dado)
          printf("casos diarios en el dia %s en la localidad de %s es de %i\n",
                 lista->informe.hora,lista->informe.localidad,
                 lista->informe.confirmados - anterior->informe.confirmados);
          fprintf(fp, "%i %i\n", indiceArchivo++,
                  lista->informe.confirmados - anterior->informe.confirmados);
          anterior = lista;
          }
      }
  //Actualizo el puntero
        anterior = lista;
    }
  }
  free(fechaA);
  free(fechaB);
  free(fechaC);
  fclose(fp);
}

/**
* Funcion que devuelve los casos confirmados acumulados en un intervalo
* de tiempo por medio de un archivo
*/
void casos_acumulado_intervalo(Notificaciones* agenda, char* localidad,
                               char* departamento, char* fechaInicio,
                               char* fechaFin) {
  //Abro el archivo a escribir
  FILE* fp = fopen("AcumuladosAGraficar.dat", "w");
  //Calculo el indice mediante la funcion hash
  unsigned int hashCalculado = hash(localidad);
  unsigned int indice = hashCalculado % agenda->capacidad;

  ListaEnlazada * nuevaLista = agenda->entradas[indice].listaEnlazada;
  //Con el mismo principio de casos_acumulados ordeno la LSE
  ListaEnlazada* lista = glist_selection_sort(nuevaLista);
  //Variables donde se va a guardar las dos fechas extremos del intervalo
  //y la fecha del nodo en el momento de recorrer la LSE
  Calendario* fechaA = malloc(sizeof(struct Fecha_));
  Calendario* fechaB = malloc(sizeof(struct Fecha_));
  Calendario* fechaC = malloc(sizeof(struct Fecha_));
  fecha_a_numero(fechaInicio, fechaA);
  fecha_a_numero(fechaFin, fechaC);
  int indiceArchivo = 0;
  //Recorro la LSE
  for (;lista != NULL; lista = lista->siguiente) {
  //Sea un nodo de esa localidad transformo su fecha para ver si esta
  //en el intervalo
    if (strcmp(lista->informe.localidad, localidad) == 0 &&
        strcmp(lista->informe.departamento, departamento) == 0) {
      fecha_a_numero(lista->informe.hora, fechaB);
      if ((strcmp(lista->informe.hora, fechaInicio) == 0 &&
           strcmp(lista->informe.hora, fechaFin) == 0) ||
          (compare_dates(fechaB, fechaA) == 1 &&
          (compare_dates(fechaC, fechaB) == 1 ||
           strcmp(fechaFin,lista->informe.hora) == 0))) {
  //Si esta en el intervalo lo escribo en el archivo
        fprintf(fp, "%i %i\n", indiceArchivo++, lista->informe.confirmados);
      }
    }
  }
  //Libero la memoria pedida para comparar fechas
  free(fechaA);
  free(fechaB);
  free(fechaC);
  fclose(fp);
}

/**
* Funcion que desglosa el contenido del archivo( pasando como
* argumento su nombre) y carga en la tabla hash todo el contenido del mismo
*/
void parser_cvs(char* nombreArchivo, Notificaciones* agenda) {
  //Abro el archivo con el nombre parametro
  FILE* archivo = fopen(nombreArchivo, "r");
  //Si no se puede abrir hay un error con fopen
  if( archivo == NULL){
    printf("Error al abrir el archivo .csv\n");
    return;
  }
  //Variable que nos permite saltear la primera linea del .csv
  //que son los nombres de los campos, que no nos interesan
  int primera_linea = 0;
  //Recorro el archivo
  while (!feof(archivo)) {
  //Cargo en una cadena la primera linea del archivo
    char *linea = calloc(sizeof(char), BUFFER_SIZE);
    fscanf(archivo, "%1023[^\n\r]", linea);
  //Si no se trata de la linea que contiene los nombres de los argumentos
  //parseo esa linea e inserto su contenido
    if (primera_linea != 0) {
      Diario* dia = parsear_linea(linea);
      notificaciones_insertar(agenda, dia->hora, dia->departamento,
                              dia->localidad, dia->confirmados,
                              dia->descartados, dia->enEstudio,
                              dia->notificaciones);
      free(dia);
    } else {
      primera_linea =1;
      }
  //Como hasta el final de linea y libero la memoria que pedi para
  //guardar la linea del archivo
    comer_finales_de_linea(archivo);
    free(linea);
  }
  fclose(archivo);
}

/**
* Funcion que va a leer de la consola la entrada del usuario y
* la divide en dos partes. La primera que es la operacion y la
* segunda que son los argumentos
*/
void parsear_comando_y_operacion(char* comando, char* operacion) {
  //Inicializo la cadena donde voy a guardar la instruccion a realizar
  char* com = calloc(27 + 1, sizeof(char));
  int indice = 0;
  //Guardo la instruccion
  for (;comando[indice] != '\0' && comando[indice] != ' ' &&
       comando[indice] != '\n';indice++) {
    com[indice] = comando[indice];
  }
  com[indice] = '\0';
  int indiceComando = 0;
  //Guardo los argumentos de la instruccion
  for (;comando[indice] != '\0' && comando[indice] != '\n';
      indice++, indiceComando++ ){
    operacion[indiceComando] = comando[indice];
  }
  operacion[indiceComando] = '\0';
  strcpy(comando,com);
  free(com);
}

/**
* Funcion que agarra la entrada de los argumentos y segun la operacion
* los almacenamos, ya que cada instruccion en el interprete recibe
* sus argumentos en un orden predefinido
*/
void parser_elim_acum_dup(char* operacion, char* fecha, char* localidad,
                          char* dpto, int* confirmados, int* descartados,
                          int* enEstudio, char* fechaInicio,char* fechaFinal,
                          char* nombreArchivo , int parser){
  //Si se trata tanto de las instrucciones de carga y imprimir
  //estas reciben solo un argumento que es el nombre del archivo
  if (parser == 1) {
    char* nombreDeArchivo = calloc(27 + 1, sizeof(char));
    int indice = 1;
    int indiceNombre = 0;
    int operacionLargo = strlen(operacion);
    //Si el largo de la cadena pasada como argumento es mayor o igual a 6
    //recien ahi la guardo de caso contrario se trata del nombre ".csv"
    if (operacionLargo >= 6) {
      for (int largo = 0; largo < operacionLargo - 5 && operacion[indice] != ' ';
        indice++,indiceNombre++,largo++) {
        nombreDeArchivo[indiceNombre] = operacion[indice];
      }
  nombreDeArchivo[indiceNombre] = '\0';
  //No admito que la entrada en el nombre del archivo tenga espacios
      if (operacion[indice] == ' ') {
        printf("Error en el nombre del archivo, espacio en el nombre\n");
        free (nombreDeArchivo);
      } else {
  //Controlo que la extension del archivo pasado como argumento
  //sea .csv y guardo el nombre
          if (operacion[indice] == '.' && operacion[indice + 1] == 'c' &&
              operacion[indice + 2] == 's' && operacion[indice + 3] == 'v' &&
              operacion[indice + 4] == '\0') {
            strcat(nombreDeArchivo,".csv");
            strcpy(nombreArchivo,nombreDeArchivo);
            printf("NOMBRE DE ARCHIVO PARSER: -%s-\n",nombreArchivo);
            free(nombreDeArchivo);
          } else {
  //Error en la extension
              printf("NO ESTA LA EXTENSION REQUERIDA .csv\n");
              free(nombreDeArchivo);
            }
        }
    } else {
  //Nombre con longitud menor a "a.csv" con "a" como caracter alfanumerico
        printf("LARGO MINIMO NO ALCANZADO, MINIMAMENTE");
        printf(" EL NOMBRE ES a.cvs EN LONGITUD\n");
        free(nombreDeArchivo);
      }
  }
  //Si se trata tanto de las instrucciones de tiempo duplicacion, casos
  //acumulados o eliminar registro estas reciben estos argumentos
  //en este orden fecha localidad departamento
  if (parser == 0) {
  //Actualizo contadores y aumento el indice para omitir el ' '
    int indice = 1;
    int indiceDos = 0;
  //Guardo la fecha
    for (;isdigit(operacion[indice]) || operacion[indice] == '-' ||
         operacion[indice] == ':' || operacion[indice] == 'T';
         indice++, indiceDos++) {
      fecha[indiceDos] = operacion[indice];
    }
    fecha[indiceDos] = '\0';
  //Actualizo contadores y aumento el indice para omitir el ' '
    indice++;
    indiceDos = 0;
  //Guardo la localidad
    for (;isdigit(operacion[indice]) || (isupper(operacion[indice]) &&
        isupper(operacion[indice + 1])) || (isupper(operacion[indice]) &&
        isupper(operacion[indice - 1])) || (isupper(operacion[indice]) &&
        isupper(operacion[indice + 2]) && isupper(operacion[indice + 3])) ||
        (!isupper(operacion[indice]) && isupper(operacion[indice + 1]) &&
        isupper(operacion[indice + 2])); indice++, indiceDos++) {
      localidad[indiceDos] = operacion[indice];
    }
    localidad [indiceDos] = '\0';
  //Actualizo contadores y aumento el indice para omitir el ' '
    indice++;
    indiceDos = 0;
  //Guardo el departamento
    for (;isdigit(operacion[indice]) || islower(operacion[indice]) ||
        islower(operacion[indice + 1]) || (isupper(operacion[indice]) &&
        islower(operacion[indice + 1])) || (isupper(operacion[indice + 1]) &&
        islower(operacion[indice + 2])); indice++, indiceDos++) {
      dpto[indiceDos] = operacion[indice];
    }
    dpto[indiceDos] = '\0';
  }
  //Si la instruccion leida es buscar pico, parseo sus argumentos
  //dados en el orden de localidad departamento
  if (parser == 2) {
  //Actualizo contadores y aumento el indice para omitir el ' '
    int indice = 1;
    int indiceDos = 0;
  //Guardo la localidad
    for (;isdigit(operacion[indice]) || (isupper(operacion[indice]) &&
         isupper(operacion[indice + 1])) || (isupper(operacion[indice]) &&
         isupper(operacion[indice - 1])) || (isupper(operacion[indice]) &&
         isupper(operacion[indice + 2]) && isupper(operacion[indice + 3])) ||
         (!isupper(operacion[indice]) && isupper(operacion[indice + 1]) &&
         isupper(operacion[indice + 2])); indice++, indiceDos++) {
      localidad[indiceDos] = operacion[indice];
    }
    localidad [indiceDos] = '\0';
  //Actualizo contadores y aumento el indice para omitir el ' '
    indice++;
    indiceDos = 0;
  //Guardo el departamento
    for (;isdigit(operacion[indice]) || islower(operacion[indice]) ||
        islower(operacion[indice + 1]) || (isupper(operacion[indice]) &&
        islower(operacion[indice + 1])) || (isupper(operacion[indice + 1]) &&
        islower(operacion[indice + 2])); indice++, indiceDos++) {
      dpto[indiceDos] = operacion[indice];
    }
    dpto[indiceDos] = '\0';
  }
  //Si se trata de la instruccion agregar registro parseo en
  //orden sus argumentos fecha departamento localidad confirmados descartados
  //enEstudio
  if (parser == 3) {
  //Actualizo contadores y aumento el indice para omitir el ' '
    int indice = 1;
    int indiceDos = 0;
  //Guardo la fecha
    for (;isdigit(operacion[indice]) || operacion[indice] == '-' ||
         operacion[indice] == ':' || operacion[indice] == 'T';
         indice++, indiceDos++) {
      fecha[indiceDos] = operacion[indice];
    }
    fecha[indiceDos] = '\0';
  //Actualizo contadores y aumento el indice para omitir el ' '
    indice++;
    indiceDos = 0;
  //Guardo el departamento
    for (;isdigit(operacion[indice]) || islower(operacion[indice]) ||
         islower(operacion[indice + 1]) || (isupper(operacion[indice]) &&
         islower(operacion[indice + 1])) || (isupper(operacion[indice + 1]) &&
         islower(operacion[indice + 2])); indice++, indiceDos++) {
      dpto[indiceDos] = operacion[indice];
    }
    dpto[indiceDos] = '\0';
  //Actualizo contadores y aumento el indice para omitir el ' '
    indice++;
    indiceDos = 0;
  //Guardo la localidad
    for(;isdigit(operacion[indice]) || (isupper(operacion[indice]) &&
        isupper(operacion[indice + 1])) || (isupper(operacion[indice]) &&
        isupper(operacion[indice - 1])) || (isupper(operacion[indice]) &&
        isupper(operacion[indice + 2]) && isupper(operacion[indice + 3])) ||
        (!isupper(operacion[indice]) && isupper(operacion[indice + 1]) &&
        isupper(operacion[indice + 2])); indice++, indiceDos++) {
      localidad[indiceDos] = operacion[indice];
    }
    localidad [indiceDos] = '\0';
  //Actualizo contadores y aumento el indice para omitir el ' '
    indice++;
    indiceDos = 0;
  //Creo, guardo y convierto a entero la cadena de texto
  //correspondiente a los casos confirmados
    char* confirm = calloc(27 + 1, sizeof(char));
    for (;isdigit(operacion[indice]); indice++, indiceDos++) {
      confirm[indiceDos] = operacion[indice];
    }
    confirm[indiceDos] = '\0';
    *confirmados = (int)numero_char_a_int(confirm);
  //Actualizo contadores y aumento el indice para omitir el ' '
    indice++;
    indiceDos = 0;
  //Creo, guardo y convierto a entero la cadena de texto
  //correspondiente a los casos descartados
    char* descart = calloc(27 + 1, sizeof(char));
    for (;isdigit(operacion[indice]); indice++, indiceDos++) {
      descart[indiceDos] = operacion[indice];
    }
    descart[indiceDos] = '\0';
    *descartados = numero_char_a_int(descart);
  //Actualizo contadores y aumento el indice para omitir el ' '
    indice++;
    indiceDos = 0;
  //Creo, guardo y convierto a entero la cadena de texto
  //correspondiente a los casos en estudio
    char* estudio = calloc(27 + 1, sizeof(char));
    for (;isdigit(operacion[indice]); indice++, indiceDos++) {
      estudio[indiceDos] = operacion[indice];
    }
    estudio[indiceDos] = '\0';
    *enEstudio = numero_char_a_int(estudio);
  //Libero la memoria de las cadenas convertidas a entero
    free(estudio);
    free(descart);
    free(confirm);
  }
  //Si se trata de la funcion graficadora parseo sus argumentos con el
  //siguiente orden predefinido fechaInicio fechaFin localidad departamento
  if (parser == 4) {
  //Actualizo contadores y aumento el indice para omitir el ' '
    int indice = 1;
    int indiceDos = 0;
  //Guardo la fecha inicial del intervalo
    for (;isdigit(operacion[indice]) || operacion[indice] == '-' ||
         operacion[indice] == ':' || operacion[indice] == 'T';
         indice++, indiceDos++) {
      fechaInicio[indiceDos] = operacion[indice];
    }
    fechaInicio[indiceDos] = '\0';
  //Actualizo contadores y aumento el indice para omitir el ' '
    indice++;
    indiceDos = 0;
  //Guardo la la fecha final del intervalo
    for (;isdigit(operacion[indice]) || operacion[indice] == '-' ||
         operacion[indice] == ':' || operacion[indice] == 'T';
         indice++, indiceDos++) {
      fechaFinal[indiceDos] = operacion[indice];
    }
    fechaFinal[indiceDos] = '\0';
  //Actualizo contadores y aumento el indice para omitir el ' '
    indice++;
    indiceDos = 0;
  //Guardo la localidad
    for (;isdigit(operacion[indice]) || (isupper(operacion[indice]) &&
         isupper(operacion[indice + 1])) || (isupper(operacion[indice]) &&
         isupper(operacion[indice - 1])) || (isupper(operacion[indice]) &&
         isupper(operacion[indice + 2]) && isupper(operacion[indice + 3])) ||
         (!isupper(operacion[indice]) && isupper(operacion[indice + 1]) &&
         isupper(operacion[indice + 2]));indice++, indiceDos++) {
      localidad[indiceDos] = operacion[indice];
    }
    localidad [indiceDos] = '\0';
  //Actualizo contadores y aumento el indice para omitir el ' '
    indice++;
    indiceDos = 0;
  //Guardo el departamento
    for (;isdigit(operacion[indice]) || islower(operacion[indice]) ||
         islower(operacion[indice + 1]) || (isupper(operacion[indice]) &&
         islower(operacion[indice + 1])) || (isupper(operacion[indice + 1]) &&
         islower(operacion[indice + 2]));indice++, indiceDos++) {
      dpto[indiceDos] = operacion[indice];
    }
    dpto[indiceDos] = '\0';
  }
}

/**
* Funcion que asigna a cada instruccion un indice de ejecucion
*/
int comando_int(char* operacion, char* alias) {
  //Tenue detector de errores de input por parte del usuario
  if (operacion[1] == ' ') {
    printf("Error en los espacios...\n");
    return 10;
  }
  if (operacion[0] == ' ' && operacion[1] == '\0') {
    printf("No hay argumentos pasados o espacio al final de una instruccion\n");
    return 10;
  }
  //Operacion salir
  if (strcmp(alias, "salir") == 0 && strcmp(operacion, "") == 0 )
    return 9;
  //Operacion cargar_dataset
  if (strcmp (alias, "cargar_dataset") == 0 && strcmp(operacion,"") != 0)
    return 1;
  //Operacion imprimir_dataset
  if (strcmp (alias, "imprimir_dataset") == 0 && strcmp(operacion,"") != 0)
    return 2;
  //Operacion agregar_registro
  if (strcmp (alias, "agregar_registro") == 0 && strcmp(operacion,"") != 0)
    return 3;
  //Operacion eliminar_registro
  if (strcmp (alias, "eliminar_registro") == 0 && strcmp(operacion,"") != 0)
    return 4;
  //Operacion buscar_pico
  if (strcmp (alias, "buscar_pico") == 0 && strcmp(operacion,"") != 0)
    return 5;
  //Operacion casos_acumulados
  if (strcmp (alias, "casos_acumulados") == 0 && strcmp(operacion,"") != 0)
    return 6;
  //Operacion tiempo_duplicacion
  if (strcmp (alias, "tiempo_duplicacion") == 0 && strcmp(operacion,"") != 0)
    return 7;
  //Operacion graficar
  if (strcmp (alias, "graficar") == 0 && strcmp(operacion,"") != 0)
    return 8;
  //Si no entra en las definiciones anteriores, se trataa de un
  //erro de sintaxis generico
  return 10;
}

/**
* Funcion que vuelca el contenido de una tabla hash sobre un archivo .csv
*/
void crear_salida_cvs (char* nombreArchivo, Notificaciones* agenda) {
  //Creo el archivo con el nombre pasado como parametro
  FILE* fp = fopen(nombreArchivo, "w");
  //Guardo la capacidad de mi tabla para recorrer todas las LSE del
  //area de rebalse
  int capacidad = agenda->capacidad;
  //Como es un archivo .csv en la primera linea le inserto un cabezal con
  //el nombre de los campos a representar
  fprintf(fp, "Fecha,Departamento,Localidad,Confirmados,Descartados,");
  fprintf(fp, "En estudio,Notificaciones\n");
  //Recorro todas las LSE presentes en la tabla
  for (int indice = 0; indice < capacidad; indice++) {
    ListaEnlazada* nuevaLista = agenda->entradas[indice].listaEnlazada;
  //Por cada lista no nula, la recorro nodo a nodo, guardando en
  //mi archivo el contenido de las mismas
    for (;nuevaLista != NULL; nuevaLista = nuevaLista->siguiente) {
      fprintf(fp, "%s,%s,%s,%i,%i,%i,%i\n", nuevaLista->informe.hora,
              nuevaLista->informe.departamento, nuevaLista->informe.localidad,
              nuevaLista->informe.confirmados, nuevaLista->informe.descartados,
              nuevaLista->informe.enEstudio,
              nuevaLista->informe.notificaciones);
    }
  }
  fclose(fp);
}

/**
* Funcion que grafica los puntos presentes en el archivo AcumuladosAGraficar.csv
*/
void plot_casos_acumulados() {
  //Invoco a gnuplot
  FILE *gnuplotPipe = popen("gnuplot", "w");
  //Seteo el nombre de la grafica
  fprintf(gnuplotPipe, "set title \"Contagios Acumulados\" font \",20\"\n");
  fprintf(gnuplotPipe, "set key left box\n");
  fprintf(gnuplotPipe, "set samples 50\n");
  fprintf(gnuplotPipe, "set style data points\n");
  //Seteo el nombre de los ejes, en caso son los contagios confirmados(eje Y) y
  //el tiempo (eje X) expresado en dias posteriores a la fecha de inicio
  fprintf(gnuplotPipe, "set xlabel \"Dias siguientes a la fecha de inicio\"\n");
  fprintf(gnuplotPipe, "set ylabel \"Contagios\"\n");
  //Configuro el rango a graficar
  fprintf(gnuplotPipe, "set xrange [ 0 : * ] noreverse writeback\n");
  fprintf(gnuplotPipe, "set yrange [ 0 : * ] noreverse writeback\n");
  //Ploteo el archivo en cuestion
  fprintf(gnuplotPipe, "plot [0:*] 'AcumuladosAGraficar.dat'\n");
  fflush(gnuplotPipe);
//    sleep( 5 );
  printf("Para cerrar la grafica, aprete enter...\n");
  getchar();
  //Cierro la grafica
  fprintf(gnuplotPipe, "quit\n");
  pclose(gnuplotPipe);
}

/**
* Funcion que grafica los puntos presentes en el archivo DiariosAGraficar.csv
*/
void plot_casos_diarios() {
  //Invoco a gnuplot
  FILE *gnuplotPipe = popen("gnuplot", "w");
  //Seteo el nombre de la grafica
  fprintf(gnuplotPipe, "set title \"Casos Diarios\" font \",20\"\n");
  fprintf(gnuplotPipe, "set key left box\n");
  fprintf(gnuplotPipe, "set samples 50\n");
  fprintf(gnuplotPipe, "set style data points\n");
  //Seteo el nombre de los ejes, en caso son los contagios diarios
  //confirmados(eje Y) y el tiempo (eje X) expresado en dias posteriores
  //a la fecha de inicio
  fprintf(gnuplotPipe, "set xlabel \"Dias siguientes a la fecha de inicio\"\n");
  fprintf(gnuplotPipe, "set ylabel \"Contagios\"\n");
  //Configuro el rango a graficar
  fprintf(gnuplotPipe, "set xrange [ 0 : * ] noreverse writeback\n");
  fprintf(gnuplotPipe, "set yrange [ 0 : * ] noreverse writeback\n");
  //Ploteo el archivo en cuestion
  fprintf(gnuplotPipe, "plot [0:*] 'DiariosAGraficar.dat'\n");
  fflush(gnuplotPipe);
//    sleep( 5 );
  printf("Para cerrar la grafica, aprete enter...\n");
  getchar();
  //Cierro la grafica
  fprintf(gnuplotPipe, "quit\n");
  pclose(gnuplotPipe);
}

int main() {
  Notificaciones * agenda = notificaciones_crear();
  printf("BIENVENIDO AL INTERPRETE\n");
  char* localidad = malloc(LIMITE * sizeof(char));
  char* fecha = malloc(LIMITE * sizeof(char));
  char* departamento = malloc(LIMITE * sizeof(char));
  char* nombreArchivo = malloc(LIMITE * sizeof(char));
  int confirmados = 0;
  int descartados = 0;
  int enEstudio = 0;
  char* fechaInicio = malloc(LIMITE * sizeof(char));
  char* fechaFinal = malloc(LIMITE * sizeof(char));
  int interprete = 0;
  int las = 0;
  while (interprete == 0) {
    char* alias = malloc(LIMITE * sizeof(char));

    if(las == 19){
     strcpy(alias,"salir");
     las++;
     }
     if(las == 18){
   // strcpy(alias,"tiempo_duplicacion 2020-11-22T00:00:00-03:00 ANTONIO PINI 9 de Julio");
   strcpy(alias,"cargar_dataset recargasalida.csv");
   notificaciones_buscar(agenda,"PUTO","2020-11-22T00:00:00-03:00","9 de Julio");
     las++;
     }
    if(las == 17){
   // strcpy(alias,"tiempo_duplicacion 2020-11-22T00:00:00-03:00 ANTONIO PINI 9 de Julio");
   strcpy(alias,"imprimir_dataset recargasalida.csv");
   notificaciones_buscar(agenda,"PUTO","2020-11-22T00:00:00-03:00","9 de Julio");
     las++;
     }
         if(las == 16){
     strcpy(alias,"cargar_dataset recargados.csv");
     las++;
     }

     if(las == 15){ //ELIMINA BIEN LA DATA
     strcpy(alias,"imprimir_dataset sin.csv");
     las++;
     }

         if(las == 14){
     strcpy(alias,"eliminar_registro 2020-11-22T00:00:00-03:00 PUTOS 9 de Pepe");
     las++;
     }

          if(las == 13){ //PISA BIEN LA DATA
     strcpy(alias,"imprimir_dataset nu.csv");
     las++;
     }
              if(las == 12){
     strcpy(alias,"agregar_registro 2020-11-22T00:00:00-03:00 9 de Pepe PUTOS 400 2 5");
     las++;
     }
         if(las == 11){
     strcpy(alias,"agregar_registro 2020-11-22T00:00:00-03:00 9 de Pepe PUTOS 4 2 5");
     las++;
     }
     if(las == 10){
     strcpy(alias,"imprimir_dataset nuevo.csv");
     las++;
     }
     if(las == 9){
     strcpy(alias,"graficar 2020-06-22T00:00:00-03:00 2020-11-22T00:00:00-03:00 ROSARIO Rosario");
     las++;
     }
    if(las == 8){
     strcpy(alias,"tiempo_duplicacion 2020-11-22T00:00:00-03:00 ROSARIO Rosario");
     las++;
     }
    if(las == 7){
     strcpy(alias,"casos_acumulados 2020-11-22T00:00:00-03:00 ROSARIO Rosario");
     las++;
     }
    if(las == 6){
     strcpy(alias,"buscar_pico ANTONIO PINI 9 de Julio");
     las++;
     }
              if(las == 5){
     strcpy(alias,"casos_acumulados 2020-11-22T00:00:00-03:00 ANTONIO PINI 9 de Julio");
     las++;
     }
         if(las == 4){
     strcpy(alias,"buscar_pico ROSARIO Rosario");
     las++;
     }
    if(las == 3){
     strcpy(alias,"eliminar_registro 2020-11-22T00:00:00-03:00 PUTOS 9 de Pepe");
     las++;
     }
    if(las == 2){
     strcpy(alias,"eliminar_registro 2020-11-22T00:00:00-03:00 PUTOS 9 de Pepe");
     las++;
     }
    if(las == 1){
     strcpy(alias,"agregar_registro 2020-11-22T00:00:00-03:00 9 de Pepe PUTOS 4 2 5");
     las++;
     }
    if(las == 0){
     strcpy(alias,"cargar_dataset hola.csv");
     las++;
     }


  //  if (fgets(alias, LIMITE, stdin)){}
    char* operacion = calloc(LIMITE, sizeof(char));
    int instruccion = 0;
    if (strlen(alias) == LIMITE - 1) {
      printf("Overflow de entrada...\n");
      if (scanf("%*[^\n]")){}
      if (scanf("%*c")){}
      free (alias);
      continue;
    }
    parsear_comando_y_operacion(alias, operacion);
    instruccion = comando_int(operacion, alias);
    printf("\nALIAS: -%s-\n",alias);
    printf("\nOPERACION: -%s-\n",operacion);
    switch (instruccion) {
      case 1:
        printf("Cargar_dataset...\n");//PARSER 1
        parser_elim_acum_dup(operacion, fecha, localidad, departamento,
                             &confirmados, &descartados, &enEstudio,
                             fechaInicio, fechaFinal, nombreArchivo, 1);
        printf("NOMBRE DE ARCHIVO DE ENTRADA: -%s-\n", nombreArchivo);
        parser_cvs(nombreArchivo, agenda);
        break;
      case 2:
        printf("Imprimir_dataset...\n"); //PARSER 1
        parser_elim_acum_dup(operacion, fecha, localidad, departamento,
                             &confirmados, &descartados, &enEstudio,
                             fechaInicio, fechaFinal, nombreArchivo, 1);
        printf("NOMBRE DE ARCHIVO DE SALIDA: -%s-\n", nombreArchivo);
        crear_salida_cvs(nombreArchivo,agenda);
        break;
      case 3:
        printf("Agregar Registro...\n");//PARSER 3
        parser_elim_acum_dup(operacion, fecha, localidad, departamento,
                             &confirmados, &descartados, &enEstudio,
                             fechaInicio, fechaFinal, nombreArchivo, 3);
        printf("AGREGAR REGISTRO\nFECHA: -%s-\nLOCALIDAD: -%s-\nDEPARTAMENTO: -%s-\nCONFIRMADOS: -%i-\nDESCARTADOS: -%i-\nEN ESTUDIO: -%i-\n", fecha,localidad,departamento,confirmados,descartados,enEstudio);
        notificaciones_insertar(agenda, fecha, departamento, localidad,
                                confirmados, descartados, enEstudio,
                                confirmados + descartados + enEstudio);
        notificaciones_buscar(agenda, localidad, fecha, departamento);
        break;
      case 4:
        printf("Eliminar Registro...\n");//PARSER 0
        parser_elim_acum_dup(operacion, fecha, localidad, departamento,
                             &confirmados, &descartados, &enEstudio,
                             fechaInicio, fechaFinal, nombreArchivo, 0);
        printf("FECHA A ELIMINAR: -%s-\n",fecha);
        printf("LOCALIDAD A ELIMINAR: -%s-\n",localidad);
        printf("DEPARTAMENTO A ELIMINAR: -%s-\n",departamento);
        notificaciones_eliminar(agenda, localidad, fecha, departamento);
        notificaciones_buscar(agenda, localidad, fecha, departamento);
        break;
      case 5:
        printf("Buscar Pico...\n");//PARSER 2
        parser_elim_acum_dup(operacion, fecha, localidad, departamento,
                             &confirmados, &descartados, &enEstudio,
                             fechaInicio, fechaFinal, nombreArchivo, 2);
        printf("BUSCAR PICO:\nLOCALIDAD: -%s-\nDEPARTAMENTO: -%s-\n",localidad,departamento);
        buscar_pico(agenda, localidad, departamento);
        break;
      case 6:
        printf("Casos acumulados...\n");//PARSER 0
        parser_elim_acum_dup(operacion, fecha, localidad, departamento,
                             &confirmados, &descartados, &enEstudio,
                             fechaInicio, fechaFinal, nombreArchivo, 0);
        printf("FECHA: -%s-\n",fecha);
        printf("LOCALIDAD: -%s-\n",localidad);
        printf("DEPARTAMENTO: -%s-\n",departamento);
        int casos = casos_acumulados(agenda, localidad, fecha, departamento);
        printf("En la localidad %s hubo %i contagios totales hasta el dia %s\n",
               localidad, casos, fecha);
        break;
      case 7:
        printf("Tiempo Duplicacion...\n");//PARSER 0
        parser_elim_acum_dup(operacion, fecha, localidad, departamento,
                             &confirmados, &descartados, &enEstudio,
                             fechaInicio, fechaFinal, nombreArchivo, 0);
        printf("FECHA: -%s-\n",fecha);
        printf("LOCALIDAD: -%s-\n",localidad);
        printf("DEPARTAMENTO: -%s-\n",departamento);
        tiempo_duplicacion(agenda, fecha, localidad, departamento);
        break;
      case 8:
        printf("Graficando...\n"); //PARSER 4
        parser_elim_acum_dup(operacion, fecha, localidad, departamento,
                             &confirmados, &descartados, &enEstudio,
                             fechaInicio, fechaFinal, nombreArchivo, 4);
        printf("FECHA INICIO GRAFICA: -%s-\n",fechaInicio);
        printf("FECHA FINAL GRAFICA: -%s-\n",fechaFinal);
        printf("LOCALIDAD GRAFICA: -%s-\n",localidad);
        printf("DEPARTAMENTO GRAFICA: -%s-\n",departamento);
        casos_acumulado_intervalo(agenda, localidad ,departamento, fechaInicio, fechaFinal);
        casos_diarios_intervalo(agenda, localidad, departamento, fechaInicio, fechaFinal);
        //plot_casos_acumulados();
        //plot_casos_diarios();
        break;
      case 9:
        printf("Saliendo del programa...\n");
        interprete = 1;
        break;
      case 10:
        printf("ERROR DE SINTAXIS\n");
        break;
    }
    free(operacion);
    free(alias);
  }
  free(localidad);
  free(fecha);
  free(departamento);
  free(nombreArchivo);
  free(fechaInicio);
  free(fechaFinal);
  notificaciones_destruir(agenda);
  return 0;
}
