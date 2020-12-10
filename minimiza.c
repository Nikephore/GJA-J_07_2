#include "minimiza.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DECIMAL 10
#define CHECK 3
#define FIN -2

/*FUNCIONES PRIVADAS*/
int *claseSort(int *c, int *t, int ini, int fin);
int valor_maximo(int *a);
int numero_valores(int* array);
int numero_valores_distintos(int* array);
void free_double_pointer(int** tabla, int limite);
int comparar_arrays(int *p, int *q);
int estadoEnClase(int x, int *check, int *tabla);
int *arraySort(int * number);
char* itoa(int val, int base);
void arrayPrint(char *n, int *a);
int* arrayIni(int tam);
int *arrayAjustar(int *checkpoint, int tam);


/*FUNCION PRINCIPAL*/
AFND * AFNDMinimiza(AFND * afnd){
    int i, j, k, l, numcheck, cont, tcont = 0;
    int ini, fin, x = 0, flag = 0, limite=0;
    int * tabla = NULL;
    int * checkpoint = NULL;
    int * clases = NULL;
    int ** ttran = NULL;
    /*char *nombre = "";*/
    AFND *ret = NULL;

    /*Nestros estados del AFD pasarán a estar en un array de enteros*/
    tabla =arrayIni(AFNDNumEstados(afnd));

    printf("tamanio inicial: %d\n", numero_valores(tabla));

    /*usamos marcadores para dividir las clases del automata*/
    checkpoint = arrayIni(CHECK);

    printf("tamanio check: %d\n", numero_valores(checkpoint));

    /*El primer marcador corresponde al primer indice de tabla | x = 0*/
    checkpoint[x] = 0;
    x++;
    printf("tamanio check: %d\n", numero_valores(checkpoint));
    /*Introducimos los valores del afd en nuestro array*/
    j = 0;
    for(i = 0; i < AFNDNumEstados(afnd); i++){
        if(AFNDTipoEstadoEn(afnd, i) == FINAL || AFNDTipoEstadoEn(afnd, i) == INICIAL_Y_FINAL){
            tabla[j]= i;
            printf("%d \n", tabla[j]);
            j++;
        }

    }
    printf("\n");

    /*El segundo marcador corresponde al valor donde se dividen las clases | x = 1*/
    checkpoint[x] = j;
    x++;

    for(i = 0; i < AFNDNumEstados(afnd); i++){
        if(AFNDTipoEstadoEn(afnd, i) == NORMAL || AFNDTipoEstadoEn(afnd, i) == INICIAL){
            tabla[j]= i;
            printf("%d \n", tabla[j]);
            j++;
        }

    }
    printf("\n");
    /*El ultimo marcador corresponde al ultimo indice de tabla | x = 2*/
    checkpoint[x] = j;
    x++;
    /*x = 3*/

    arrayPrint("check\t", checkpoint);
    printf("tamanio inicial: %d\n", numero_valores(checkpoint));
    arrayPrint("tabla\t", tabla);
    printf("tamanio inicial: %d\n", numero_valores(tabla));


    /**
     * Ejemplo, en [0, 1, 2, 3, 4, 5, 6, 7] Siendo de 0 a 4 estados NO finales y de 5 a 7 estados finales
     *
     * checkpoint[0] = 0; checkpoint[1] = 5; checkpoint[2] = 8
     *
     * clase 0 : [0, 1, 2, 3, 4] ; clase 1 : [5, 6, 7]
     *
     *
     * De esta forma checkpoint[i+1] - checkpoint[i] corresponde al numero de estados que hay en la clase i
     *
     * checkpoint[0+1] - checkpoint[0] = 5 - 0 = 5 (que es el numero de estados que hay en la clase 0)
     *
    **/

   /*Mientras no se creen nuevas clases realizamos el bucle*/
    while (tcont != numero_valores(checkpoint)){

        /**
         * igualamos el contador al numero de marcadores.
         * Si el numero de marcadores aumenta se seguira ejecutando el bucle
         * En caso contrario el bucle dejará de ejecutarse.
        */
        tcont = numero_valores(checkpoint);


        for(i = 0; i < numero_valores(checkpoint) - 1; i++){

            /*Creamos una tabla de transiciones para la clase que estamos comprobando*/
            ttran = (int**)calloc(checkpoint[i+1] - checkpoint[i], sizeof(int*));

            for(j = 0; j < checkpoint[i+1] - checkpoint[i]; j++){

                ttran[j] = arrayIni(AFNDNumSimbolos(afnd));

                for(k = 0; k < AFNDNumSimbolos(afnd); k++){

                    for (l = 0; l < AFNDNumEstados(afnd); l++){

                        if(AFNDTransicionIndicesEstadoiSimboloEstadof(afnd, j, k, l) == 1){

                        /*Creamos la matriz con las clases de los estados a los que transita nuestro estado actual*/
                        ttran[j][k] = estadoEnClase(l, checkpoint, tabla);
                        }
                    }
                }
            }

            /*Creamos un array con el cual crearemos la division de clases de la clase actual*/
            clases = arrayIni(checkpoint[i+1] - checkpoint[i]);

            cont = 0;
            clases[0] = cont;
            cont ++;
            for(j = 1; j < checkpoint[i+1] - checkpoint[i]; j++){

                flag = 1;

                /**
                 * Comparamos cada estado con los anteriores
                 * Si su array de transiciones es igual al de otro estado
                 * pertenece a su misma clase. Si no coincide con ningun
                 * otro array creamos una clase nueva con el valor de cont.
                 * */
                for(k = j-1; k >= 0; k--){

                    if(comparar_arrays(ttran[j], ttran[k]) == 0){
                        clases[j] = clases[k];
                        flag = 0;
                        break; /*salir del for actual*/
                    }
                }

                if(flag == 1) {
                    clases[j] = cont;
                    cont++;
                }

            }

            /*Si de la clase actual se han sacado 2 clases o mas hacemos lo siguiente*/
            if(cont > 1){

                /**
                 * Reordenamos la tabla para que cada clase este separada del resto
                 * Ejemplo:
                 * Tabla [0, 1, 2, 3, 4, 5, 6, 7]
                 * Clase 0 [0, 1, 2, 3, 4]
                 *
                 * Tras ejecutar el algoritmo la clase se divide (por ejemplo) en [0, 1, 4] y [2, 3]
                 *
                 * Reordenamos la tabla para que quede asi:
                 *
                 * Tabla [|0, 1, 4,| 2, 3,| 5, 6, 7|]
                 *       c0=0     c3=3   c1=5     c2=8
                 * */

                ini = checkpoint[i];
                fin = checkpoint[i+1];

                tabla = claseSort(clases, tabla, ini, fin);
                clases = arraySort(clases);

                numcheck = numero_valores(checkpoint) + numero_valores_distintos(clases) - 1;
                printf("\n");
                arrayPrint("CHEckAWUI", checkpoint);
                printf("numcheck:%d\tnumvalores:%d\n",numcheck,numero_valores(checkpoint));
                printf("Nuero distintos:%d\n",numero_valores_distintos(clases));

                checkpoint = arrayAjustar(checkpoint,numcheck);
                /*(int*)realloc(checkpoint, numcheck);*/
                /*Ajuste de tam*/



                l = ini;
                for(k = 0; k < numero_valores(clases) - 1; k++){
                    if(clases[k] != clases[k+1]){
                        checkpoint[x] = l+1;
                        x++;
                        l++;
                    }
                }

                checkpoint = arraySort(checkpoint);

            }

            free(clases);
            limite=checkpoint[i+1] - checkpoint[i];
            free_double_pointer(ttran,limite);
        }
    }

    /*Crear nuevo AFD con los valores de Tabla*/

    ret = AFNDNuevo(AFNDNombre(afnd), 1, AFNDNumSimbolos(afnd));

    /*Insertamos los símbolos*/
    for(i = 0; i < AFNDNumSimbolos(ret); i++){
        AFNDInsertaSimbolo(ret, AFNDSimboloEn(afnd, i));
    }

    /*Insertamos los estados
    for(i = 0; i < numero_valores(checkpoint) -1; i++){
        nombre = itoa(i, DECIMAL);
        nombre = strcat(nombre, "q");


        AFNDInsertaEstado(ret, nombre, AFNDTipoEstadoEn(afnd, tabla[checkpoint[i]]));
    }
    */
    /*Insertamos las transiciones
    for(i = 0; i < numero_valores(checkpoint) -1; i++){
        for(j = 0; j < AFNDNumSimbolos(ret); j++){
            for(k = 0; k < numero_valores(checkpoint) -1; k++){
                if(AFNDTransicionIndicesEstadoiSimboloEstadof(afnd, tabla[checkpoint[i]], j, tabla[checkpoint[j]]) == 1){
                    AFNDInsertaTransicion(ret, nombre, AFNDSimboloEn(ret, j), nombre);
                }
            }

        }
    }
    */

    AFNDInsertaEstado(ret, "TEST", INICIAL_Y_FINAL);
    AFNDInsertaTransicion(ret, "TEST", AFNDSimboloEn(ret, 0), "TEST");

    free(checkpoint);
    free(tabla);

    return ret;
}


int * claseSort(int *c, int *t, int ini, int fin){
    int i, j, k, l;
    int *aux = NULL;

    if(!c || !t || ini < 0 || fin < 0) {
        printf("Error en claseSort!!\n");
        return NULL;
    }

    aux=arrayIni(numero_valores(c));

    printf("Ini:%d\n", ini);
    arrayPrint("CLASES", c);
    arrayPrint("T:", t);
    printf("numero_valores:%d\n",numero_valores(c) );
    l = 0;
    for(i = 0; i <= valor_maximo(c); i++){
        k = ini;

        /*[0, 0, 0, 1, 0, 0, 0, 1, 0, ]*/
        /*AUX: []*/
        for(j = 0; j < numero_valores(c); j++){
            if(c[j] == i){
                aux[l] = t[k];
                l++;
            }
            k++;
        }
    }
    arrayPrint("AUX:", aux);


    for(i = 0, k=ini; i < numero_valores(c); i++,k++){
        t[k] = aux[i];
    }

    arrayPrint("TABLA2.0", t);

    free(aux);

    return t;
}

int valor_maximo(int *a){
    int top = 0, i;

    for(i = 0; i < numero_valores(a); i++){
        if(a[i] > top) top = a[i];
    }

    return top;
}

int numero_valores(int* array){
    int i=0;
    if (!array) return -1;
    while(array[i]!=-1){
        if(array[i]==FIN){
            break;
        }

        i++;
    }
    return i;
}

/*La funcion unicamente funciona si el array está ordenado*/
int numero_valores_distintos(int* array){
    int i, x = 0;
    if(!array) return -1;

    arrayPrint("FUNCION VALORES DISTINTOS", array);
    for(i = 0; i < numero_valores(array) - 1; i++){
        if(array[i] != array[i+1]) x++;
    }

    return x;
}

void free_double_pointer(int ** dp, int limite){
    int i = 0;
    if(!dp) return;

    while(i<limite && dp[i]!=NULL ){
        free(dp[i]);
        i++;
    }

    free(dp);

    return;
}

int comparar_arrays(int *p, int *q){
    int i;

    if(!p || !q){
        printf("Error en comparar arrays!!\n");
        return -1;
    }

    if(numero_valores(p) != numero_valores(q)){
        printf("Los arrays no tienen el mismo numero de elementos\n");
        return -1;
    }

    for(i = 0; i < numero_valores(p); i++){
        if(p[i] != q[i]) return 1;
    }

    return 0;
}

int estadoEnClase(int x, int *check, int *tabla){
    int i, j;
    if(x < 0 || !check || !tabla){
        printf("Error en estadoEnClase!!\n");
        return -1;
    }

    for(i = 0; i < numero_valores(tabla); i++){
        if(x == tabla[i]){
            for(j = 0; j < numero_valores(check); j++){
                if(i < check[j]){
                    return j-1;
                }
            }
        }
    }

    return -1;
}

int * arraySort(int * number){
    int i, j, a;

    if(!number){
        printf("Error en arraySort!!\n");
        return NULL;
    }

    for (i = 0; i < numero_valores(number); i++){
        for (j = i + 1; j < numero_valores(number); j++){
            if (number[i] > number[j]){
                    a =  number[i];
                    number[i] = number[j];
                    number[j] = a;
            }
        }
    }

    return number;
}

char* itoa(int val, int base){

	static char buf[32] = {0};

	int i = 30;

	for(; val && i ; --i, val /= base)

		buf[i] = "0123456789abcdef"[val % base];

	return &buf[i+1];

}

void arrayPrint(char *n, int *a){
    int i;
    if(!n || !a) return;

    printf("%s: [", n);

    for(i = 0; i < numero_valores(a); i++){
        printf("%d, ", a[i]);
    }
    printf("]\n");

    return;
}

int* arrayIni(int tam){
    int *array= NULL, i=0;
    if(tam<0) return NULL;

    array = (int*)calloc(tam+1, sizeof(int));
    if(!array) return NULL;

    for(i=0;i<tam;i++){
        array[i]=-1;
    }
    array[i]=FIN;

    return array;
}


int *arrayAjustar(int *checkpoint, int tam){
    int i=0, *array=NULL;
    if(!checkpoint) return NULL;

    array=arrayIni(tam);

    printf("\n");
    arrayPrint("checkpoint", checkpoint);
    printf("TAM+:%d",tam);

    for(i=0;i<numero_valores(checkpoint);i++){
        array[i]=checkpoint[i];
    }
    arrayPrint("arrayAjustar", array);
    return array;
}

/*clases[0, 0, 1]*/
/*tabla [|5, 7,| 6|]*/

/*
    clase 1 [1, 3, 5]
    clase 0 [0, 2, 6]
    trans 0 - . - 5 (1)
    trans 0 - + - 6 (0)
    trans 2 - . - 2 (0)
    trans 2 - + - 3 (1)
    trans 6 - . - 1 (1)
    trans 6 - + - 0 (0)
    -------------------
    clase 0 [0, 6]
    clase 1 [1, 3, 5]
    clase 2 [2]
*/
