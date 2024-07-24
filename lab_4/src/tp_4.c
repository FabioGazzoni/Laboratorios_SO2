#include "../lib/imagesUtil.h"
#include <stdio.h>
#include <stdlib.h>

int getArg(int argc, char const *argv[]);
void freeAlloc();

GDALDatasetH hDataset;
Bands bands;
Bands fBands;

int main(int argc, char const *argv[])
{
    int numThreads = getArg(argc, argv);
    //Obtiene todos los registros de los tipo de archivos a leer.
    GDALAllRegister();
    hDataset = openImageFile("../Sentinel2_20230208T140711_10m.tif");
    bands = getBandsImage(hDataset);
    fBands = filterBands(bands, numThreads);
    saveImage(fBands);
    
    freeAlloc();
    return 0;
}

int getArg(int argc, char const *argv[]){
    if (argc <2)
    {
        perror("Cantidad de argumentos inválidos, ingrese la cantidad de hilos para trabajar.");
        exit(EXIT_FAILURE);
    }

    if (atoi(argv[1]) <= 0)
    {
        perror("Cantidad de hilos inválidos.");
        exit(EXIT_FAILURE);
    }
    
    return atoi(argv[1]);
}

void freeAlloc(){
    CPLFree(bands.red);
    CPLFree(bands.green);
    CPLFree(bands.blue);

    CPLFree(fBands.red);
    CPLFree(fBands.green);
    CPLFree(fBands.blue);

    GDALClose(hDataset);
}