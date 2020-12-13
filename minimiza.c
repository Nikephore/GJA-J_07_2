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
int *arrayAjustar(int *checkpoint, int tamI, int tamF);
int obtenerEstado(AFND *afnd, int *tabla, int *checkpoint, int x);
int * bfs (AFND *afd);


/*FUNCION PRINCIPAL*/
AFND * AFNDMinimiza(AFND * afnd){
    int i, j, k, l, numcheck, cont, tcont = 0;
    int ini, fin, x = 0, flag = 0, limite=0;
    int * tabla = NULL;
    int * tablabfs = NULL;
    int * checkpoint = NULL;
    int * clases = NULL;
    int ** ttran = NULL;
    AFND *ret = NULL;

    tablabfs = bfs(afnd);

    arrayPrint("TablaBFS", tablabfs);

    /*tablabfs = (int*)realloc(tablabfs, numero_valores(tablabfs)+1);
    tablabfs[numero_valores(tablabfs)] = FIN;*/

    /*Nestros estados del AFD pasarán a estar en un array de enteros*/
    tabla = arrayIni(numero_valores(tablabfs));

    /*usamos marcadores para dividir las clases del automata*/
    checkpoint = arrayIni(CHECK);

    /*El primer marcador corresponde al primer indice de tabla | x = 0*/
    checkpoint[x] = 0;
    x++;
    
    /*Introducimos los valores del afd en nuestro array*/
    j = 0;
    for(i = 0; i < numero_valores(tablabfs); i++){
        if(AFNDTipoEstadoEn(afnd, tablabfs[i]) == FINAL || AFNDTipoEstadoEn(afnd, tablabfs[i]) == INICIAL_Y_FINAL){
            tabla[j]= tablabfs[i];
            j++;
        }

    }
    printf("\n");

    /*El segundo marcador corresponde al valor donde se dividen las clases | x = 1*/
    checkpoint[x] = j;
    x++;

    for(i = 0; i < numero_valores(tablabfs); i++){
        if(AFNDTipoEstadoEn(afnd, tablabfs[i]) == NORMAL || AFNDTipoEstadoEn(afnd, tablabfs[i]) == INICIAL){
            tabla[j]= tablabfs[i];
            j++;
        }

    }
    printf("\n");
    /*El ultimo marcador corresponde al ultimo indice de tabla | x = 2*/
    checkpoint[x] = j;
    x++;
    /*x = 3*/

    free(tablabfs);

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
            ini = checkpoint[i];
            for(j = 0; j < checkpoint[i+1] - checkpoint[i]; j++){
                
                ttran[j] = arrayIni(AFNDNumSimbolos(afnd));

                printf("\nEstado %d\n", j);
                for(k = 0; k < AFNDNumSimbolos(afnd); k++){
                    
                    printf("Simbolo %s\n", AFNDSimboloEn(afnd, k));
                    for (l = 0; l < AFNDNumEstados(afnd); l++){

                        if(AFNDTransicionIndicesEstadoiSimboloEstadof(afnd, tabla[ini], k, l) == 1){
                        /*Creamos la matriz con las clases de los estados a los que transita nuestro estado actual*/
                        ttran[j][k] = estadoEnClase(l, checkpoint, tabla);
                        printf("Est: %d | ttran: %d\n", l, ttran[j][k]);
                        
                        }
                        
                    }
                    
                }
                ini++;
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

            arrayPrint("Clases antes de cont > 1", clases);

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
                printf("numcheck:%d\tnumvalores:%d\n",numcheck, numero_valores(checkpoint));

                checkpoint = arrayAjustar(checkpoint, numero_valores(checkpoint), numcheck);       
                l = ini;

                arrayPrint("Clases precheck", clases);
                for(k = 0; k < numero_valores(clases) - 1; k++){
                    if(clases[k] != clases[k+1]){
                        /*printf("CMP clases %d: %d %d\n",k, clases[k], clases[k+1]);*/
                        checkpoint[x] = l+1;
                        x++;      
                    }
                    l++;
                }

                arrayPrint("Check post realloc", checkpoint);

                checkpoint = arraySort(checkpoint);

            }

            free(clases);
            limite = checkpoint[i+1] - checkpoint[i];
            free_double_pointer(ttran,limite);
        }
    }

    /*Crear nuevo AFD con los valores de Tabla*/

    ret = AFNDNuevo(AFNDNombre(afnd), numero_valores(checkpoint)-1, AFNDNumSimbolos(afnd));

    /*Insertamos los símbolos*/
    for(i = 0; i < AFNDNumSimbolos(ret); i++){
        AFNDInsertaSimbolo(ret, AFNDSimboloEn(afnd, i));
    }

    arrayPrint("TABLAFINAL", tabla);
    arrayPrint("CHECKFINAL", checkpoint);

    /*Insertamos los estados*/
    for(i = 0; i < numero_valores(checkpoint) -1; i++){

        AFNDInsertaEstado(ret, AFNDNombreEstadoEn(afnd, tabla[checkpoint[i]]), obtenerEstado(afnd, tabla, checkpoint, i));

    }
    
    /*Insertamos las transiciones*/
    for(i = 0; i < numero_valores(tabla); i++){
        for(j = 0; j < AFNDNumSimbolos(ret); j++){
            for(k = 0; k < numero_valores(tabla); k++){
                if(AFNDTransicionIndicesEstadoiSimboloEstadof(afnd, tabla[i], j, tabla[k]) == 1){
                    ini = estadoEnClase(tabla[i], checkpoint, tabla);
                    fin = estadoEnClase(tabla[k], checkpoint, tabla);

                    printf("ini: %d i: %d fin: %d k: %d\n", ini, i, fin, k);
                    printf("Tabla[i]: %d\n", tabla[i]);
                    
                    AFNDInsertaTransicion(ret, AFNDNombreEstadoEn(afnd, tabla[checkpoint[ini]]), AFNDSimboloEn(ret, j), AFNDNombreEstadoEn(afnd, tabla[checkpoint[fin]]));
                    
                }
            }
        }
    }



    /*AFNDInsertaTransicion(ret, "|q0|", AFNDSimboloEn(ret, 0), "|q1|");*/

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

    printf("CsortIni:%d\n", ini);
    arrayPrint("CLASES", c);
    arrayPrint("TablaSinOrdenar", t);
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
    arrayPrint("TablaAuxiliar", aux);


    for(i = 0, k=ini; i < numero_valores(c); i++,k++){
        t[k] = aux[i];
    }

    arrayPrint("TablaOrdenada", t);

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
    int i, x = 1;
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


    for(i = 0; i < limite; i++){
        free(dp[i]);
        dp[i] = NULL;
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

void arrayPrint(char *n, int *a){
    int i;
    if(!n || !a) return;

    printf("%s: [", n);

    for(i = 0; i < numero_valores(a)+1; i++){
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
        array[i] = -1;
    }
    array[i] = FIN;

    return array;
}


int *arrayAjustar(int *checkpoint, int tamI, int tamF){
    int i;
    if(!checkpoint) return NULL;

    printf("TAM_I:%d\n",tamI);
    printf("TAM_F:%d\n",tamF);

    checkpoint = (int*)realloc(checkpoint, (tamF+1)*sizeof(int));
    for(i= tamI; i<tamF; i++){
        checkpoint[i] = -1;
        printf("Pos: %d Ci: %d\n", i, checkpoint[i]);
    }
    
    checkpoint[i] = FIN;
    printf("Pos: %d Ci: %d\n", i, checkpoint[i]);
    printf("numvalores %d\n", numero_valores(checkpoint));

    return checkpoint;
}

int obtenerEstado(AFND *afnd, int *tabla, int *checkpoint, int x){
    int i, estado = NORMAL;

    if(!afnd || !tabla || !checkpoint || x<0) return -1;

    for(i = checkpoint[x]; i < checkpoint[x+1]; i++){
        if(AFNDTipoEstadoEn(afnd, tabla[i]) == INICIAL_Y_FINAL){
            estado = INICIAL_Y_FINAL;

            return estado;
        }

        if(AFNDTipoEstadoEn(afnd, tabla[i]) == FINAL){
            estado = FINAL;
        }
        if(AFNDTipoEstadoEn(afnd, tabla[i]) == INICIAL){
            estado = INICIAL;
        }

    }

    return estado;
}

int * bfs (AFND *afd){
    int i, j, k, x = 0;
    int *ret = NULL;
    Bool flag = FALSE;

    ret = arrayIni(AFNDNumEstados(afd));
    if(!ret) return NULL;

    arrayPrint("BFS", ret);

    for(i = 0; i < AFNDNumEstados(afd); i++){
        flag = FALSE;

        for(j = 0; j < AFNDNumSimbolos(afd); j++){
            for(k = 0; k < AFNDNumEstados(afd); k++){
               if(AFNDTransicionIndicesEstadoiSimboloEstadof(afd, k, j, i) == 1){
                   printf("Transicion: %d -> %d -> %d\n", k, j, i);
                   flag = TRUE;
               } 
            }
        }

        if(flag == TRUE){
            ret[x] = i;
            x++;
        }

        printf("ret[x] = %d  i= %d\n", ret[x], i);
    }

    return ret;
}

