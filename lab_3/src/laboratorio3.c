#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

int XDIM = 100;
int YDIM = 100;

/**
 * @brief Genera una matriz con valores booleanos aleatorios.
 */
double **generate_matrix(void)
{
    srand(1);
    double **array;
    array = (double **)malloc((unsigned long)XDIM * sizeof(double *));
    if (array == NULL)
    {
        perror("Error al alocar memoria");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < XDIM; i++)
    {
        array[i] = (double *)malloc((unsigned long)YDIM * sizeof(double));
        if (array[i] == NULL)
        {
            perror("Error al alocar memoria");
            exit(EXIT_FAILURE);
        }
        for (int j = 0; j < YDIM; j++)
            array[i][j] = (double)(rand() % 100);
    }

    return array;
}

/**
 * @brief Computa kernell con la matriz array.
 */
void compute(double **arr, int kern[3][3], FILE *f_results)
{
    double accum = 0;
    for (int i = 0; i < XDIM; i++)
        for (int j = 0; j < YDIM; j++)
        {
            fprintf(f_results, "processing: %d - %d \n", i, j);
            if (i >= 1 && j >= 1 && i < XDIM - 1 && j < YDIM - 1)
            {
                accum = 0;
                for (int k = 0; k < 3; k++)
                    for (int l = 0; l < 3; l++)
                        accum += ((kern[l][k] * arr[i + (l - 1)][j + (k - 1)]) * 0.004 + 1);
            }
            arr[i][j] = accum;
        }
}

/**
 * @brief Almacena los resultados de la operación en un archivo.
 */
void save_results(double **arr, FILE *f_results)
{
    for (int i = 0; i < XDIM; i++)
        for (int j = 0; j < YDIM; j++)
            fprintf(f_results, "array[%d][%d] = %f\n", i, j, arr[i][j]);
}

/**
 * @brief Genera el archivo de escritura.
 */
FILE *generate_file(void)
{
    FILE *f_results;
    f_results = fopen("results", "w");
    setvbuf(f_results, NULL, _IOFBF, (size_t)(32 * XDIM * YDIM));
    if (f_results == NULL)
    {
        perror("No se pudo abrir el archivo.");
        exit(EXIT_FAILURE);
    }
    else
        return f_results;
}

/**
 * @brief Se libera la memoria alocada y se cierra el archivo results.
 */
void finish_program(FILE *f_results, double **arr)
{
    fclose(f_results);
    for (int i = 0; i < XDIM; i++)
    {
        free(arr[i]);
    }
    free(arr);
}

int main(void)
{
    FILE *f_results = generate_file();

    double **arr;
    int kern[3][3] = {{0, -1, 0}, {-1, 5, -1}, {0, -1, 0}};
    arr = generate_matrix();
    compute(arr, kern, f_results);
    save_results(arr, f_results);

    finish_program(f_results, arr);
    return 0;
}