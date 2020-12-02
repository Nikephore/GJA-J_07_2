#include "minimiza.h"

/*FUNCIONES PRIVADAS*/
void* int_copy(const int* p);
int int_print(FILE* pf, const int* p);
int int_cmp(const int* p, const int* q);
void int_free(int* p);

int numero_valores(int* array);
void free_double_pointer(int** tabla);


/*FUNCION PRINCIPAL*/
AFND * AFNDMinimiza(AFND * afnd){
    int i, j, k, l, clase, cont, tcont;
    int row = 0, clm = 0, flag = 0;
    int * tabla = NULL;
    int * checkpoint = NULL;
    int ** ttran = NULL;
    int * clases = NULL;
    int * aux = NULL;

    /*Nestros estados del AFD pasarán a estar en un array de enteros*/
    tabla = (int*)calloc(AFNDNumEstados(afnd), sizeof(int));

    /*usamos marcadores para dividir las clases del automata*/
    checkpoint = (int*)calloc(3, sizeof(int));

    /*El primer marcador corresponde al primer indice de tabla*/
    checkpoint[0] = 0;

    /*Introducimos los valores del afd en nuestro array*/
    j = 0;
    for(i = 0; i < AFNDNumEstados(afnd); i++){
        if(AFNDTipoEstadoEn(afnd, i) == FINAL || AFNDTipoEstadoEn(afnd, i) == INICIAL_Y_FINAL){
            tabla[j]= i;
            j++; 
        } 
    }
    /*El segundo marcador corresponde al valor donde se dividen las clases*/
    checkpoint[1] = j;


    for(i = 0; i < AFNDNumEstados(afnd); i++){
        if(AFNDTipoEstadoEn(afnd, i) == NORMAL || AFNDTipoEstadoEn(afnd, i) == INICIAL){
            tabla[j]= i;
            j++; 
        }
    }
    /*El ultimo marcador corresponde al ultimo indice de tabla*/
    checkpoint[2] = j;

    /**
     * Ejemplo, en [0, 1, 2, 3, 4, 5, 6, 7] Siendo de 0 a 4 estados NO finales y de 5 a 7 estados finales
     * 
     * checkpoint[0] = 0; checkpoint[1] = 5; checkpoint[2] = 8 
     * 
     * clase 0 : [0, 1, 2, 3, 4] ; clase 1 : [5, 6, 7]
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

                ttran[j] = (int**)calloc(AFNDNumSimbolos(afnd), sizeof(int*));

                for(k = 0; k < AFNDNumSimbolos(afnd); k++){

                    for (l = 0; l < AFNDNumEstados(afnd); l++){

                        if(AFNDTransicionIndicesEstadoiSimboloEstadof(afnd, j, k, l) == 1){
                        ttran[j][k] = EstadoEnClase(l);
                        }
                    }   
                }
            }
        
  
            clases = (int*)calloc(checkpoint[i+1] - checkpoint[i], sizeof(int));
            clases[0] = 0;
            cont = 1;

            for(j = 1; j < numero_valores(clases); j++){

                flag = 1;

                for(k = j-1; k <= 0; k--){
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

            if(cont > 1){ /*Si la clase inicial se divide en mas de una*/
                tabla = claseSort(clases, tabla, checkpoint[i], checkpoint[j]);
                clases = arraySort(clases);

                /*Falta realizar gestion de los checkpoints*/
            }
            free_double_pointer(ttran);
        }      
    }

    free_double_pointer(tabla);
}


int * claseSort(int *c, int *t, int ini, int fin){
    int i, j, k, l;
    int *aux = NULL;

    if(!c || !t || ini < 0 || fin < 0) {
        printf("Error en claseSort!!\n");
        return NULL;
    }

    for(i = 0; i < valor_maximo(c); i++){
        k = ini;
        l = 0;
        for(j = 0; j < numero_valores(c); j++){
            if(c[j] == i){
                aux[l] = t[k];
                l++;
            }
            k++;
        }
    }

    for(i = 0; i < numero_valores(c); i++){
        t[i] = aux[i];
        free(aux[i]);
        aux[i] = NULL;
    }

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
    int i = 0;
    if(!array) return -1;

    while (array[i] != NULL) i++;

    return i;
}

void free_double_pointer(int ** dp){
    int i = 0;
    if(!dp) return;

    while(dp[i] != NULL) free(dp[i]); i++;

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

int estadoEnClase(int x){
    if(x < 0){
        printf("Error en estadoEnClase!!\n");
        return -1;
    }

    /**
     * FALTA POR IMPLEMENTAR ESTA FUNCION AUXILIAR
     */

}

int * arraySort(int * number){
    int i, j, a;

    if(!number){
        printf("Error en arraySort!!\n");
        return -1;
    }

    for (i = 0; i < numero_valores(number); ++i){
        for (j = i + 1; j < numero_valores(number); ++j){
            if (number[i] > number[j]){
                    a =  number[i];
                    number[i] = number[j];
                    number[j] = a;
            }
        }
    }

    return number;
}


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
