#include "parser.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
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
