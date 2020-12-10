#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <ctype.h>
#include "hash.h"
#include "LSE.h"
#include "parser.h"
#define BUFFER_SIZE 1024
#define LIMITE 250

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
          fprintf(fp, "%i %i\n", indiceArchivo++, lista->informe.confirmados);
          anterior = lista;
        } else {
  //Si el nodo anterior para esa localidad no es nulo, entonces calculamos
  //los casos diarios( esta en el intervalo dado)
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
     strcpy(alias,"agregar_registro 2020-11-22T00:00:00-03:00 9 de Pepé PUTOS 400 2 5");
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
      free(alias);
      free(operacion);
      continue;
    }
    parsear_comando_y_operacion(alias, operacion);
    instruccion = comando_int(operacion, alias);
    switch (instruccion) {
      case 1:
        printf("Cargar_dataset...\n");//PARSER 1
        parser_elim_acum_dup(operacion, fecha, localidad, departamento,
                             &confirmados, &descartados, &enEstudio,
                             fechaInicio, fechaFinal, nombreArchivo, 1);
        parser_cvs(nombreArchivo, agenda);
        break;
      case 2:
        printf("Imprimir_dataset...\n"); //PARSER 1
        parser_elim_acum_dup(operacion, fecha, localidad, departamento,
                             &confirmados, &descartados, &enEstudio,
                             fechaInicio, fechaFinal, nombreArchivo, 1);
        crear_salida_cvs(nombreArchivo,agenda);
        break;
      case 3:
        printf("Agregar Registro...\n");//PARSER 3
        parser_elim_acum_dup(operacion, fecha, localidad, departamento,
                             &confirmados, &descartados, &enEstudio,
                             fechaInicio, fechaFinal, nombreArchivo, 3);
        notificaciones_insertar(agenda, fecha, departamento, localidad,
                                confirmados, descartados, enEstudio,
                                confirmados + descartados + enEstudio);
        break;
      case 4:
        printf("Eliminar Registro...\n");//PARSER 0
        parser_elim_acum_dup(operacion, fecha, localidad, departamento,
                             &confirmados, &descartados, &enEstudio,
                             fechaInicio, fechaFinal, nombreArchivo, 0);
        notificaciones_eliminar(agenda, localidad, fecha, departamento);
        break;
      case 5:
        printf("Buscar Pico...\n");//PARSER 2
        parser_elim_acum_dup(operacion, fecha, localidad, departamento,
                             &confirmados, &descartados, &enEstudio,
                             fechaInicio, fechaFinal, nombreArchivo, 2);
        buscar_pico(agenda, localidad, departamento);
        break;
      case 6:
        printf("Casos acumulados...\n");//PARSER 0
        parser_elim_acum_dup(operacion, fecha, localidad, departamento,
                             &confirmados, &descartados, &enEstudio,
                             fechaInicio, fechaFinal, nombreArchivo, 0);
        int casos = casos_acumulados(agenda, localidad, fecha, departamento);
        printf("En la localidad %s hubo %i contagios totales hasta el dia %s\n",
               localidad, casos, fecha);
        break;
      case 7:
        printf("Tiempo Duplicacion...\n");//PARSER 0
        parser_elim_acum_dup(operacion, fecha, localidad, departamento,
                             &confirmados, &descartados, &enEstudio,
                             fechaInicio, fechaFinal, nombreArchivo, 0);
        tiempo_duplicacion(agenda, fecha, localidad, departamento);
        break;
      case 8:
        printf("Graficando...\n"); //PARSER 4
        parser_elim_acum_dup(operacion, fecha, localidad, departamento,
                             &confirmados, &descartados, &enEstudio,
                             fechaInicio, fechaFinal, nombreArchivo, 4);
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
