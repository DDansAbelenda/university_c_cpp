#include "../rpc.h"
#include <stdlib.h>
#include <stdio.h>

/*
Autor: Daniel Dans Abelenda
*/
typedef struct
{
    reportable_t parent;

    char str [100]; // cadena de caractéres de entrada que se analiza
    int resultado;  // valor donde se guarda si es palíndromo (1) o no (0)
} my_struct_t;

/*Función que carga los parámetros de entrada del buffer*/
void *parse_parameters(void *data)
{
    const char *buf = (const char *)(data);

    my_struct_t *d = (my_struct_t *)(malloc(sizeof(my_struct_t)));

    if (d)
    {   //Se lee la cadena del primer parámetro y se asigna a str
        sscanf(buf, "%s", d->str);
    }

    return (void *)d;
}

/*En el cuerpo de esta función lo que se hace es procesar la cadena str para determinar si 
es palindorme, en caso de ser cierto se guarda en resultado = 1, caso contrario resultado = 0.*/
void *do_work(void *data)
{
    my_struct_t *d = (my_struct_t *)(data);
    
    int i = 0, j = 0; // Declaramos dos índices
      j = strlen(d->str) - 1; // Inicializamos el índice final

    while (i <= j) { // Iniciamos el bucle while
        if (d->str[i] != d->str[j]) { /*Si los caracteres son diferentes, la cadena no es un 
                                      palíndromo*/
            d->resultado = 0;
            return data;
        }
        i++; // Incrementamos el índice inicial
        j--; // Decrementamos el índice final
    }
    d->resultado = 1; // Si el bucle termina, la cadena es un palíndromo

    return data;
}

/*En esta función se genera el resultado en consola a partir de los datos procesados anteriormente*/
reportable_t *report(void *data)
{
    my_struct_t *d = (my_struct_t *)(data);

    d->parent.data = (char *)(malloc(255 * sizeof(char)));

    //Contenido a imprimir, para decidir si es palíndromo se tiene en cuenta el valor de resultado
    snprintf(d->parent.data, 255, "La cadena: '%s' %s\n", d->str, 
    d->resultado?"es palindrome":"no es palindrome");
    d->parent.len = strlen(d->parent.data);

    return (reportable_t *)(data);
}

void clean_up(void *params, void *result, reportable_t *report)
{
    if (report && report->data)
    {
        free(report->data);
    }

    if (params)
    {
        free(params);
    }
}
