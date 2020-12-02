/*
 * file: list.c
 * @author: miguel.manzanom@estudiante.uam.es
 * @author: franciscoj.marin@estudiante.uam.es
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "list.h"

typedef struct _NodeList { /* EdD privada, necesaria para implementar lista */
 void* info;
 struct _NodeList *next;
} NodeList; /* Tipo NodeList privado */


struct _List {
 NodeList *last; /*La LEC apunta al último nodo y el último al primero*/
 destroy_element_function_type destroy_element_function;
 copy_element_function_type copy_element_function;
 print_element_function_type print_element_function;
 cmp_element_function_type cmp_element_function;
};

/*############################################################################*/

/******************************************************************************/
/* Funcion: nodeList_ini                                                      */
/* Inicializa el nodo de la lista con la informacion actual y del siguiente   */
/******************************************************************************/
NodeList *nodeList_ini(){
  NodeList *nl=NULL;

  nl=(NodeList*)malloc(sizeof(NodeList));
  if(!nl) return NULL;

  nl->info=NULL;
  nl->next=NULL;

  return nl;
}

/******************************************************************************/
/* Funcion: nodeList_destroy                                                  */
/* Libera la memoria reservada para el nodo de la lista                       */
/******************************************************************************/
void nodeList_destroy(List *list,NodeList *n){

  if(!n || !list){
    return;
  }

  list->destroy_element_function(n->info);

  free(n);
}

/*############################################################################*/


/******************************************************************************/
/* Funcion: list_ini                                                          */
/* Inicializa la lista reservando memoria e inicializa todos sus elementos.   */
/******************************************************************************/
List* list_new (destroy_element_function_type f1, copy_element_function_type f2, print_element_function_type f3,
cmp_element_function_type f4){

  List *list=NULL;
  if(!f1 || !f2 || !f3 || !f4) return NULL;


  list=(List*)malloc(sizeof(List));
  if(!list) return NULL;

  list->last=NULL;
  list->destroy_element_function=f1;
  list->copy_element_function=f2;
  list->print_element_function=f3;
  list->cmp_element_function=f4;

  return list;
}

/******************************************************************************/
/* Funcion: list_destroy                                                      */
/* Libera la lista, liberando todos sus elementos.                            */
/******************************************************************************/
void list_free (List* list){
  int tamano_lista = 0, i;

  if(!list) return;

  tamano_lista = listSize(list);
  for(i=0; i< tamano_lista; i++){
    list->destroy_element_function(list_popFront(list));
  }
  free(list);

  return;
}

/******************************************************************************/
/* Funcion: list_pushFront                                                  */
/* Inserta al principio de la lista realizando una copia de la información    */
/******************************************************************************/
Status list_pushFront (List* list, const void *pelem){

  NodeList *n=NULL;

  if(!list || !pelem) return ERROR;

  n=nodeList_ini();
  if(!n) return ERROR;

  list->destroy_element_function(n->info);
  n->info=list->copy_element_function(pelem);

  if(!n->info){
    nodeList_destroy(list,n);
    return ERROR;
  }

  n->next=list->last;
  list->last=n;

  return OK;
}

/******************************************************************************/
/* Funcion: list_pushBack                                                   */
/* Inserta al final de la lista realizando una copia de la información        */
/******************************************************************************/
Status list_pushBack (List* list, const void *pelem){

  NodeList *n=NULL,*aux=NULL;

  if(!list || !pelem) return ERROR;

  n=nodeList_ini();
  list->destroy_element_function(n->info);
  n->info=list->copy_element_function(pelem);

  if(!n->info){
    nodeList_destroy(list,n);
    return ERROR;
  }

  if(list_Empty(list)==TRUE){
    list->last=n;
    return OK;
  }

  aux=list->last;
  while(aux->next!=NULL){
    aux=aux->next;
  }

  aux->next=n;

  return OK;
}

/******************************************************************************/
/* Funcion: list_insertInOrder                                                */
/* Inserta en orden en la lista realizando una copia del elemento.            */
/******************************************************************************/
Status list_insertInOrder (List *list, const void *pelem){

  NodeList *n=NULL,*aux=NULL;
  int tam=0,cmp=-1;

  if(!list || !pelem) return ERROR;

  if(list_Empty(list)==TRUE){
    list_pushBack(list,pelem);
    return OK;
  }

  tam=listSize(list);

  if(tam==1){
    n=list->last;
    cmp=list->cmp_element_function(n->info,pelem);

    if(cmp==0 || cmp==1){
      list_pushBack(list,pelem);
      return OK;
    }
    else{
      list_pushFront(list,pelem);
      return OK;
    }
  }


  if(tam>1){
    aux=list->last;
    if(list->cmp_element_function(aux->info,pelem)==2){
      list_pushFront(list,pelem);
      return OK;
    }
    while(aux->next!=NULL){
      aux=aux->next;
    }
    if(list->cmp_element_function(aux->info,pelem)==1){
      list_pushBack(list,pelem);
      return OK;
    }
    else if(list)
    aux=NULL;
    aux=list->last;
    while(aux->next!=NULL){
      if(list->cmp_element_function(aux->info,pelem)==2){
        n=aux;
        aux->info=list->copy_element_function(pelem);
        aux->next=n;
        return OK;
      }
      else{
        aux=aux->next;
      }
    }
   }





  return ERROR;
}

/******************************************************************************/
/* Funcion: list_popFront                                                 */
/* extrae del principio de la lista, devolviendo directamente el puntero al   */
/* campo info del nodo extraído, nodo que finalmente es liberado.             */
/* OJO: tras guardar la dirección del campo info que se va a devolver y       */
/* antes de liberar el nodo, pone el campo info del nodo a NULL,              */
/* para que no siga apuntando a la info a devolver y, por tanto,              */
/* no la libere al liberar el nodo                                            */
/******************************************************************************/
void * list_popFront (List* list){

  NodeList *n=NULL;
  void *ele=NULL;

  if(!list || list_Empty(list)==TRUE) return NULL;

  n=list->last;
  ele=n->info;
  n->info=NULL;
  list->last=n->next;

  nodeList_destroy(list,n);

  return ele;
}

/******************************************************************************/
/* Funcion: list_extractLast                                                  */
/* extrae del final de la lista, devolviendo directamente el puntero al       */
/* campo info del nodo extraído, nodo que finalmente es liberado.             */
/* OJO: tras guardar la dirección del campo info que se va a devolver y       */
/* antes de liberar el nodo, pone el campo info del nodo a NULL,              */
/* para que no siga apuntando a la info a devolver y, por tanto,              */
/* no la libere al liberar el nodo                                            */
/******************************************************************************/
void * list_popBack (List* list){

  NodeList *n=NULL;
  void *ele=NULL;

  if(!list || list_Empty(list)==TRUE) return NULL;

  if(listSize(list)==1){
    ele=list->last->info;
    list->last->info=NULL;
    nodeList_destroy(list,n);
    list->last=NULL;
    return ele;
  }

  n=list->last;
  while(n->next->next!=NULL){
    n=n->next;
  }

  ele=n->next->info;
  n->next->info=NULL;
  nodeList_destroy(list,n);
  n->next=NULL;

  return ele;
}

/******************************************************************************/
/* Funcion: list_Empty                                                      */
/* Comprueba si una lista está vacía o no.                                    */
/******************************************************************************/
Bool list_Empty (const List* list){

  if(!list) return TRUE;

  if(!list->last)return TRUE;

  return FALSE;
}

/******************************************************************************/
/* Funcion: list_get                                                          */
/* Devuelve la información almacenada en el nodo i-ésimo de la lista.         */
/* En caso de error, devuelve NULL.                                           */
/******************************************************************************/
void* list_getElementInPos (const List* list, int index){

  NodeList *n=NULL;
  void *ele=NULL;
  int cont=0;

  if(!list || index<0 || list_Empty(list)==TRUE) return NULL;

  n=list->last;

  while(cont!=index){
    n=n->next;
    cont++;
  }

  ele=n->info;
  
  return ele;
}

/******************************************************************************/
/* Funcion: listSize                                                         */
/* Devuelve el número de elementos que hay en una lista.                      */
/******************************************************************************/
int listSize (const List* list){
  int cont=1;
  NodeList *n=NULL;

  if(!list)return -1;

  if(list_Empty(list)==TRUE) return 0;

  n=list->last;
  
  while(n->next!=NULL){
    n=n->next;
    cont++;
  }

  /*nodeList_destroy((List*)list,n);*/
  return cont;
}

/******************************************************************************/
/* Funcion: list_print                                                        */
/* Imprime una lista devolviendo el número de caracteres escritos.            */
/******************************************************************************/
int listPrint (FILE *fd, const List* list){

  int k=0;
  NodeList* n=NULL;

  if(!fd || !list) return -1;

  if(listSize(list)==0) return 0;

  if(listSize(list)==1){
    return k=list->print_element_function(fd,list->last->info);
  }

  n=list->last;

  while(n->next!=NULL){
    k+=list->print_element_function(fd,n->info);
    n=n->next;
  }

  k+=list->print_element_function(fd,n->info);

  return k;
}
void* list_copy(const List* p) {
  /*
    List* copy = NULL;
    if(!p) return NULL;

    copy = (int *)malloc(sizeof(List));
    if(!copy);

    *copy = *(List*)p;
  */
    return (void*)p;
}


int list_cmp(const List* p, const List* q) {
    if(!p || !q) return -2;

    return 0;
}
