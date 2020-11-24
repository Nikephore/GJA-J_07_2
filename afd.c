#include <stdlib.h>
#include <string.h>
#include "afd.h"
#include "list.h"


struct _Estado {
    int pos;
    char* nombre;
    TipoEstado tipo;
    List* transiciones;
    int * estados_afnd;
};

struct _Simbolo {
    int pos;
    char* nombre;
};

struct _Transicion {
    Simbolo* simbolo;
    Estado* estado_f;
};

struct _AFD {
    char* nombre;
    int num_simbolos;
    int num_estados;
    List* simbolos;
    List* estados;
    List* e_finales;
    Estado* e_inicial;
};

AFD* AFDNuevo (char* nombre) {
    AFD* afd = NULL;
    if(!nombre) return NULL;

    afd = (AFD*)calloc(1, sizeof(AFD));
    if(!afd) return NULL;


    afd->nombre=(char*)calloc(sizeof(char),strlen(nombre)+1);
    strcpy(afd->nombre,nombre);
    afd->simbolos = list_new((destroy_element_function_type)simbolo_free, (copy_element_function_type)simbolo_copy, (print_element_function_type)simbolo_print, (cmp_element_function_type)simbolo_cmp);
    afd->estados = list_new((destroy_element_function_type)estado_free,(copy_element_function_type)estado_cpy, (print_element_function_type)estado_print, (cmp_element_function_type)estado_cmp);
    afd->num_simbolos = listSize(afd->simbolos);
    afd->num_estados = listSize(afd->estados);
    afd->e_finales = list_new((destroy_element_function_type)estado_free,(copy_element_function_type)estado_cpy, (print_element_function_type)estado_print, (cmp_element_function_type)estado_cmp);
    afd->e_inicial = NULL;

    return afd;
}

void AFDElimina (AFD* afd) {
    if(!afd) return;

    free(afd->nombre);
    list_free(afd->simbolos);
    list_free(afd->estados);
    list_free(afd->e_finales);
    if(afd->e_inicial!=NULL){
        estado_free(afd->e_inicial);
    }

    free(afd);

    return;
}

void AFDImprime (FILE* pf, AFD* afd) {
    if(!pf || !afd) return;

    fprintf(pf, "Nombre: %s\nNumEstados: %d\nNumSimbolos: %d\n", afd->nombre, afd->num_estados, afd->num_simbolos);

    fprintf(pf, "SIMBOLOS:\n");
    listPrint(pf, afd->simbolos);
    fprintf(pf, "ESTADOS:\n");
    listPrint(pf, afd->estados);

    fprintf(pf, "ESTADOS INICIAL Y FINAL/ES:\n");
    estado_print(pf, afd->e_inicial);
    listPrint(pf, afd->e_finales);

    return;
}

AFD * AFDInsertaSimbolo(AFD * afd, char * simbolo) {
    Simbolo* s = NULL;
    if(!afd || !simbolo) return NULL;

    if(AFDExisteSimbolo(afd, simbolo)) return afd;

    s = (Simbolo*)malloc(sizeof(Simbolo));
    if(!s) return NULL;

    s->nombre=(char*)calloc(strlen(simbolo)+1,sizeof(char));
    if(!s->nombre) return NULL;

    strcpy(s->nombre, simbolo);
    s->pos = afd->num_simbolos;

    list_pushBack(afd->simbolos, s);
    afd->num_simbolos = listSize(afd->simbolos);

    simbolo_free(s);
    return afd;
}

Estado * AFDInsertaEstado(AFND* afnd, AFD * afd, char * nombre, int tipo) {
    Estado* e = NULL;

    if(!afd || !nombre) return NULL;

    e = (Estado*)calloc(1, sizeof(Estado)); 
    if(!e) return NULL;

    e->nombre =(char*)calloc(strlen(nombre)+1,sizeof(char));

    strcpy(e->nombre, nombre);
    e->pos = afd->num_estados;
    e->tipo = tipo;
    e->transiciones = list_new((destroy_element_function_type)transicion_free, (copy_element_function_type)transicion_copy, (print_element_function_type)transicion_print, (cmp_element_function_type)transicion_cmp);
    e->estados_afnd = (int*)calloc(AFNDNumEstados(afnd),sizeof(int));
    if(!e->estados_afnd){
        printf("fallo en R.M estados\n");
        return NULL;
    }
    for(int i=0; i<AFNDNumEstados(afnd); i++){
        e->estados_afnd[i] = -1;
    }

    list_pushBack(afd->estados, e);

    if(tipo == INICIAL){ /*Es un estado inicial*/
        afd->e_inicial = e;
    }
    else if(tipo == FINAL){ /*Es un estado final*/
        list_pushBack(afd->e_finales, e);
    }
    else if(tipo == INICIAL_Y_FINAL){ /*Es un estado inicial_y_final*/
        afd->e_inicial = e;
        list_pushBack(afd->e_finales, e);
    }

    afd->num_estados = listSize(afd->estados);

    return e;
}

void AFDInsertaEstadoAFND(AFND* afnd,Estado * e, int e_afnd) {
    int i=0;
    if(!e || e_afnd== -1) return;

    i=0;
    while(e->estados_afnd[i] != -1 && i<AFNDNumEstados(afnd)){
        i++;
    }
    e->estados_afnd[i]= e_afnd;

    return ;
}

Transicion * AFDInsertaTransicion(AFD * afd, Estado *ei ,char * nombre_simbolo_entrada, char * nombre_estado_f ) {
    Transicion* t = NULL;

    Simbolo* s = NULL;
    Estado* ef = NULL;

    if(!afd || !ei || !nombre_estado_f || !nombre_simbolo_entrada) return NULL;

    t = (Transicion*)calloc(1, sizeof(Transicion));
    if(!t){
        printf("Fallo al reservar memeria en Transicion**************************\n");
    }


    s = AFDExisteSimbolo(afd, nombre_simbolo_entrada);
    ef = AFDExisteEstado(afd, nombre_estado_f);

    t->simbolo = s;

    t->estado_f = ef;

    list_pushBack(ei->transiciones, t);

    return t;
}

Simbolo* AFDExisteSimbolo(AFD * afd, char * simbolo) {
    int i;
    Simbolo* s = NULL;
    if(!afd || !simbolo) return NULL;

    for(i=0; i < afd->num_estados; i++){
        s = list_getElementInPos(afd->simbolos, i);
        if(strcmp(s->nombre, simbolo) == 0){
            return s;
        }
        /*simbolo_free(s);*/
    }

    return NULL;
}

Estado* AFDExisteEstado(AFD * afd, char * estado) {
    int i;
    Estado* e = NULL;
    if(!afd || !estado) return NULL;

    for(i=0; i < afd->num_estados; i++){
        e = AFDEstadoEn(afd, i);
        if(strcmp(e->nombre, estado) == 0){
            return e;
        }
    }

    return NULL;
}

char* AFDNombreNuevoEstado(AFND * afnd, int * e_actuales, int limite) {
    int i=0, j=0;
    Bool es_igual = FALSE;
    char* ret=NULL;
    if(!e_actuales) return NULL;

    ret=(char*)calloc(MAX, sizeof(char));
    if(!ret) return NULL;

    while(i<limite){
        if(e_actuales[i]!=-1){
            for(j=0; j<i; j++){
                if(e_actuales[i] == e_actuales[j]){
                    es_igual = TRUE;
                }
            }

            if(es_igual == FALSE){
                strcat(ret,AFNDNombreEstadoEn(afnd, e_actuales[i]));
            }

            es_igual = FALSE;
        }
        i++;
    }

    return ret;
}

Estado * AFDEstadoEn(AFD * afd, int pos) {
    Estado * e = NULL;
    if(!afd || pos == -1) return NULL;

    e = list_getElementInPos(afd->estados, pos);

    return e;
}

Simbolo * AFDSimboloEn(AFD * afd, int pos) {
    Simbolo * s = NULL;
    if(!afd || pos == -1) return NULL;

    s = list_getElementInPos(afd->simbolos, pos);

    return s;
}

int AFDNumSimbolos(AFD * afd) {
    if(!afd) return -1;

    return afd->num_simbolos;
}

int AFDNumEstados(AFD * afd) {
    if(!afd) return -1;

    return afd->num_estados;
}

int AFDNumTransiciones(Estado * e) {
    if(!e) return -1;

    return listSize(e->transiciones);
}

int AFDNumEstadosAFND(Estado * e) {
    int i=0;
    if(!e) return -1;
    while(e->estados_afnd[i] != -1){
        i++;
    }

    return i;
}

int AFDTipoEstadoEn(AFD * afd, int pos) {
    Estado * e = NULL;
    int tipo=0;
    if(!afd) return -1;

    e = list_getElementInPos(afd->estados, pos);
    tipo=e->tipo;

    /*estado_free(e);*/
    return tipo;
}

char * AFDNombre(AFD * afd) {
    if(!afd) return NULL;

    return afd->nombre;
}

int*  AFDObtenerEstadosAFND(Estado * e) {
    if(!e) return NULL;

    return e->estados_afnd;
}

AFND * AFNDTraduce(AFD * afd) {
    AFND * afnd = NULL;
    Estado * e = NULL;
    Simbolo * s = NULL;
    Transicion * t = NULL;
    int i;
    if(!afd) return NULL;

    afnd = AFNDNuevo(afd->nombre, afd->num_estados, afd->num_simbolos);
    if(!afnd){
        printf("El AFND nuevo falla\n");
        return NULL;
    }

    /*Insercion de simbolos*/
    for(i=0; i<afd->num_simbolos; i++){

        s = list_popFront(afd->simbolos);
        if(!s) return NULL;

        afnd=AFNDInsertaSimbolo(afnd, s->nombre);
        /*simbolo_free(s);*/
    }

    /*Insercion de estados*/
    for(i=0;i<listSize(afd->estados);i++){
        e = list_getElementInPos(afd->estados,i);
        if(!e) return NULL;

        /*Primero tenemos que sacar todos los estados y luego ya vemos las trasiciones*/
        AFNDInsertaEstado(afnd,e->nombre, e->tipo);
    }

    while(list_Empty(afd->estados)==FALSE){

        e = list_popFront(afd->estados);
        if(!e) return NULL;

        while(list_Empty(e->transiciones)==FALSE){

            t = list_popFront(e->transiciones);
            if(!t) return NULL;
            
            /*Usado para debug*/
            /*transicion_print(stdout,t);
            printf("Tam lista: %d\n",listSize(e->transiciones) );
            printf("IMPRIMIMOS TRANSICION: %s, %s, %s",AFDNombreEstado(e),AFDNombreSimbolo(t->simbolo),AFDNombreEstado(t->estado_f ));
            AFNDImprime(stdout,afnd);
            printf("\n");*/
            afnd=AFNDInsertaTransicion(afnd,AFDNombreEstado(e),AFDNombreSimbolo(t->simbolo),AFDNombreEstado(t->estado_f));
            /*printf("TRANSICION HECHA\n");*/

            
        }

        
    }

    /*Establece como estado inicial del autómata el que se haya declarado como estado inicial*/
    AFNDInicializaEstado (afnd);

    return afnd;
}




void int_free(int* p) {
    if(!p) return;
    free(p);
}

void* int_copy(const int* p) {
    int* copy = NULL;
    if(!p) return NULL;

    copy = (int *)malloc(sizeof(int));
    if(!copy);

    *copy = *(int*)p;

    return (void*)copy;
}

int int_print(FILE* pf, const int* p) {
    int ret = 0;

    if(!pf || !p){
        printf("NANANAIN");
        return -1;
    }

    ret += fprintf(pf, "%d", *(int*)p);
    return ret;
}

int int_cmp(const int* p, const int* q) {
    if(!p || !q) return -2;

    if(*(int *)p < *(int *)q)
      return -1;
    else if(*(int *)p == *(int *) q)
      return 0;
    else
      return 1;
}


void simbolo_free(Simbolo* p) {
    if(!p) return;

    free((char*)p->nombre);
    free(p);

    return;
}

void* simbolo_copy(const Simbolo* p) {
    Simbolo *aux=NULL;
    if(!p) return NULL;
    aux = (Simbolo*)calloc(1,sizeof(Simbolo));
    if(!aux) return NULL;

    aux->nombre=(char*)calloc(strlen(p->nombre)+1,sizeof(char));
    if(!aux->nombre) return NULL;

    strcpy(aux->nombre,p->nombre);
    aux->pos=p->pos;

    return aux;
}

int simbolo_print(FILE* pf, const Simbolo* p){
    if(!pf || !p) return -1;

    fprintf(pf, "%s\n",(char*)p->nombre);
    return 0;
}

int simbolo_cmp(const Simbolo* p, const Simbolo* q){
    int cmp=0;
    if(!p || !q) return -1;

    cmp=strcmp(p->nombre,q->nombre);
    if(cmp==0 && p->pos == q->pos){
        return 0;
    }
    return -2;
}

char *AFDNombreSimbolo(Simbolo*s){
    if(!s) return NULL;

    return s->nombre;
}

char *AFDNombreSimboloEn(AFD *afd, int pos){
    if(!afd || pos==-1) return NULL;

    return AFDNombreSimbolo(list_getElementInPos(afd->simbolos,pos));
}

void estado_free(Estado* p) {
    if(!p) return;

    free(p->nombre);
    list_free(p->transiciones);
    free(p->estados_afnd);
    free(p);

    return;
}

void* estado_cpy(const Estado* p) {
    Estado *aux=NULL;
    if(!p) return NULL;

    aux=(Estado*)calloc(1, sizeof(Estado));
    if(!aux) return NULL;
    aux->pos=p->pos;

    aux->nombre=(char*)calloc(strlen(p->nombre)+1,sizeof(char));
    strcpy(aux->nombre,p->nombre);
    aux->tipo=p->tipo;
    aux->transiciones=p->transiciones;
    aux->estados_afnd=p->estados_afnd;
    
    return aux;
}

int estado_print(FILE* pf, const Estado* p) {
    if(!pf || !p){
        printf("Estado feka\n");
        return -1;
    }

    if(p->tipo==qINICIAL){
        fprintf(pf, "->%s\n",p->nombre);
    }
    else if(p->tipo==qFINAL){
        fprintf(pf, "%s*\n",p->nombre);
    }
    else if(p->tipo==qINICIAL_Y_FINAL){
        fprintf(pf, "->%s*\n",p->nombre);
    }
    else{
        fprintf(pf, "%s\n",p->nombre);
    }
    return 0;
}

int estado_print_plus(FILE* pf, AFND *afnd, Estado* p) {
    int i;
    if(!pf || !p) return -1;

    if(p->tipo==qINICIAL){
        fprintf(pf, "->%s\n",p->nombre);
    }
    else if(p->tipo==qFINAL){
        fprintf(pf, "%s*\n",p->nombre);
    }
    else if(p->tipo==qINICIAL_Y_FINAL){
        fprintf(pf, "->%s*\n",p->nombre);
    }
    else{
        fprintf(pf, "%s\n",p->nombre);
    }

    fprintf(pf, "Pos: %d\n", p->pos);
    fprintf(pf,"transiciones:\n");
    listPrint(pf,p->transiciones);
    fprintf(pf,"Subestados:");
    for(i=0; i<AFDNumEstadosAFND(p); i++){
        if(p->estados_afnd[i]){
            fprintf(pf, "%s ", AFNDNombreEstadoEn(afnd, p->estados_afnd[i]));
        }
    }

    return 0;
}


int estado_cmp(const Estado* p, const Estado* q) {
    int cmp=0;
    if(!p || !q) return -1;

    cmp=strcmp(p->nombre,q->nombre);
    if(cmp==0 && p->pos==q->pos && p->tipo==q->tipo){
        return 0;
    }
    return -2;
}

char *AFDNombreEstado(Estado*e){
    if(!e) return NULL;

    return e->nombre;
}

int AFDTipoEstado(Estado*e){
    if(!e) return -1;;

    return e->tipo;
}

void transicion_free(Transicion* p) {
    if(!p) return;

    simbolo_free(p->simbolo);
    estado_free(p->estado_f);
    free(p);

    return;
}

void* transicion_copy(const Transicion* p) {
    Transicion* aux=NULL;
    if(!p) return NULL;

    aux = (Transicion*)calloc(1,sizeof(Transicion));
    if(!aux) return NULL;

    aux->simbolo=p->simbolo;
    aux->estado_f=p->estado_f;

    return aux;
}

int transicion_print(FILE* pf, const Transicion* p) {
    if(!pf || !p) return -1;

    fprintf(pf, "%s,%s\n",AFDNombreEstado(p->estado_f),AFDNombreSimbolo(p->simbolo));
    return 0;
}

int transicion_cmp(const Transicion* p, const Transicion* q) {
    if(!p || !q) return -1;

    if(estado_cmp(p->estado_f,q->estado_f)==0
        && simbolo_cmp(p->simbolo,q->simbolo)==0){
        return 0;
    }
    else{
        return -2;
    }
    return -1;
}
