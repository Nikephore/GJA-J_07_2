#ifndef AFD_H
#define AFD_H

#include "list.h"
#include "afnd.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX 100

/*Definicion de un enum para los tipos de estados*/
typedef enum {
    qINICIAL = 0,
    qFINAL = 1,
    qINICIAL_Y_FINAL = 2,
    qNORMAL = 3
} TipoEstado;

/*Definicion de las estructuras*/
typedef struct _Estado Estado;
typedef struct _Simbolo Simbolo;
typedef struct _Transicion Transicion;
typedef struct _AFD AFD;

AFD* AFDNuevo (char* nombre);
void AFDElimina (AFD* afd);
void AFDImprime (FILE* pf, AFD* afd);

AFD * AFDInsertaSimbolo(AFD * afd, char * simbolo);
Estado * AFDInsertaEstado(AFND* afnd,AFD * afd, char * nombre, int tipo);
void AFDInsertaEstadoAFND(AFND* afnd,Estado * e, int e_afnd);
Transicion * AFDInsertaTransicion(AFD * afd,Estado* ei, char * nombre_simbolo_entrada, char * nombre_estado_f );


Simbolo* AFDExisteSimbolo(AFD * afd, char * simbolo);
Estado* AFDExisteEstado(AFD * afd, char * estado);
Transicion* AFDExisteTransicion(AFD * afd, char * nombre_simbolo_entrada, char * nombre_estado_f);

char* AFDNombreNuevoEstado(AFND * afnd, int * e_actuales, int limite);

/* FUNCIONES PARA OBTENER INFORMACIÓN DEL AUTÓMATA */
int AFDNumSimbolos(AFD * afd);
int AFDNumEstados(AFD * afnd);
int AFDTipoEstadoEn(AFD * afd, int pos);
int AFDNumTransiciones(Estado * e);
int AFDNumEstadosAFND(Estado * e);
int * AFDObtenerEstadosAFND(Estado * e);
char *AFDNombreEstado(Estado * e);


char * AFDNombre(AFD * afd);
Estado * AFDEstadoEn(AFD * afd, int pos);
Simbolo * AFDSimboloEn(AFD * afd, int pos);

AFND * AFNDTraduce(AFD * afd);

/*Funciones para pila de enteros*/
void int_free(int* p);
void* int_copy(const int* p);
int int_print(FILE* pf, const int* p);
int int_cmp(const int* p, const int* q);

/*Funciones para pila de símbolos*/
void simbolo_free(Simbolo* p);
void* simbolo_copy(const Simbolo* p);
int simbolo_print(FILE* pf, const Simbolo* p);
int simbolo_cmp(const Simbolo* p, const Simbolo* q);
char *AFDNombreSimbolo(Simbolo*s);
char *AFDNombreSimboloEn(AFD *afd, int pos);

/*Funciones para pila de estados*/
void estado_free(Estado* p);
void* estado_cpy(const Estado* p);
int estado_print(FILE* pf, const Estado* p);
int estado_print_plus(FILE* pf, AFND *afnd, Estado* p);
int estado_cmp(const Estado* p, const Estado* q);
char *AFDNombreEstado(Estado*e);
int AFDTipoEstado(Estado*e);
/*Funciones para pila de transiciones*/
void transicion_free(Transicion* p);
void* transicion_copy(const Transicion* p);
int transicion_print(FILE* pf, const Transicion* p);
int transicion_cmp(const Transicion* p, const Transicion* q);

#endif
