#include "../lib/imagesUtil.h"

// int kernel[3][3] = {{0,-1,0}, {-1, 4, -1}, {0,-1,0}};
int kernel[3][3] = {{-1,-1,-1}, {-1, 8,-1}, {-1,-1,-1}};

/**
 * @brief Abre una imagen de formato .tif con GDAL
 * @param file Path de la imagen a abrir.
 * @return Estructura con datos de la imagen.
*/
GDALDatasetH openImageFile(const char *file){
    GDALDatasetH hDataset = GDALOpen(file, GA_ReadOnly);
    if (hDataset == NULL){
        perror("Error: no se pudo abrir el archivo");
        exit(EXIT_FAILURE);
    }

    return hDataset;
}

/**
 * @brief Se extrae de la imagen los parámetros de tamaño y matrices RGB.
 * @param hDataset Estructura con datos de la imagen.
 * @return Estructura con datos de tamaño y matrices RGB de la imagen.
*/
Bands getBandsImage(GDALDatasetH hDataset){
    Bands bands;

    GDALRasterBandH hRedBand = GDALGetRasterBand(hDataset, 1);
    GDALRasterBandH hGreenBand = GDALGetRasterBand(hDataset, 2);
    GDALRasterBandH hBlueBand = GDALGetRasterBand(hDataset, 3);

    bands.Size.x = GDALGetRasterBandXSize(hRedBand);
    bands.Size.y = GDALGetRasterBandYSize(hRedBand);

    bands.red = getMatrixBand(hRedBand, bands.Size.x, bands.Size.y);
    bands.green = getMatrixBand(hGreenBand, bands.Size.x, bands.Size.y);
    bands.blue = getMatrixBand(hBlueBand, bands.Size.x, bands.Size.y);

    return bands;
}

/**
 * @brief Se obtiene la matriz R, G, o B especificada.
 * @param hBand Estructura de datos de la banda especificada.
 * @param xSize Tamaño en alto de la imagen.
 * @param ySize Tamaño en ancho de la imagen.
 * @return Matriz R, G o B de la imagen.
*/
uint8_t *getMatrixBand(GDALRasterBandH hBand, int xSize, int ySize){
    uint8_t *bandArray;

    bandArray = CPLMalloc(sizeof(uint8_t)*(size_t)xSize*(size_t)ySize);

    CPLErr error = GDALRasterIO(hBand, GF_Read, 0, 0, xSize, ySize, bandArray, xSize, ySize, GDT_Byte, 0, 0);
    if (error != CE_None)
    {
        perror("Error: No se pudo obtener la banda en getMatrixBand()");
        exit(EXIT_FAILURE);
    }
    
    return bandArray;
}

/**
 * @brief Filtrado de imagen con el kernel definido.
 * @param bands Bandas de imagen a realizar el filtrado.
 * @param threads Cantidad de hilos a trabajar en el filtrado.
 * @return Bandas de imagen filtrada.
*/
Bands filterBands(Bands bands, int threads){
    Bands filterBands;
    filterBands.Size = bands.Size;

    double timeStart = omp_get_wtime();
    filterBands.red = matrixConvolution(bands.red, bands.Size.x, bands.Size.y, threads);
    filterBands.green = matrixConvolution(bands.green, bands.Size.x, bands.Size.y, threads);
    filterBands.blue = matrixConvolution(bands.blue, bands.Size.x, bands.Size.y, threads);
    double timeEnd = omp_get_wtime();
    printf("%f\n", timeEnd-timeStart);
    return filterBands;
}

/**
 * @brief Realiza la convolución de la matriz con el kernel definido.
 * @param matrix Matriz a realizarse la convolución (en un arreglo de xSize*ySize).
 * @param xSize Tamaño de filas de la matriz.
 * @param ySize Tamaño de columnas de la matriz.
 * @param threads Cantidad de hilos a trabajar en el filtrado.
 * @return Matriz convulsionada.
*/
uint8_t *matrixConvolution(uint8_t *matrix, int xSize, int ySize, int threads){
    uint8_t *bandFilter = CPLMalloc(sizeof(uint8_t)*(size_t)xSize*(size_t)ySize);
    memset(bandFilter, 0, sizeof(uint8_t)*(size_t)xSize*(size_t)ySize);
    int accum = 0;
    uint8_t data = 0;
    int fKernel = sizeof(kernel) / sizeof(kernel[0]);
    int cKernel = sizeof(kernel[0]) / sizeof(int);
    
    omp_set_num_threads(threads);
    #pragma omp parallel for collapse (2)
    for (int i = 1; i < xSize - 1; i++){
        for (int j = 1; j < ySize - 1; j++){
            accum = 0;
            for (int k = 0; k < fKernel; k++){
                for (int l = 0; l < cKernel; l++){
                    int x = i + (k-1);
                    int y = j + (l-1);
                    data = matrix[(xSize*x)+y];
                    accum += (kernel[k][l]*data);
                }
            }
            if (accum < 0){
                accum = 0;
            }else if(accum > 255){
                accum = 255;
            }
            bandFilter[xSize*i + j] =(uint8_t) accum;
        }
    }

    return bandFilter;
}


/**
 * @brief Guarda una imagen a partir de la información de sus bandas .tif.
 * @param bands Estructura con datos de tamaño y matrices RGB de la imagen.
*/
void saveImage(Bands bands){
    GDALDriverH hDriver = GDALGetDriverByName("GTiff");
    if (hDriver == NULL)
    {
        perror("Error: No se pudo crear hDriver");
        exit(EXIT_FAILURE);
    }
    
    char **papszOptions = NULL;
    GDALDatasetH hDataset = GDALCreate(hDriver, "edge_filter.tif", bands.Size.x, bands.Size.y, 3, GDT_Byte, papszOptions);

    GDALRasterBandH hRedBand = GDALGetRasterBand(hDataset, 1);
    saveBand(hRedBand, bands.Size.x, bands.Size.y, bands.red);
    GDALRasterBandH hGreenBand = GDALGetRasterBand(hDataset, 2);
    saveBand(hGreenBand, bands.Size.x, bands.Size.y, bands.green);
    GDALRasterBandH hBlueBand = GDALGetRasterBand(hDataset, 3);
    saveBand(hBlueBand, bands.Size.x, bands.Size.y, bands.blue);

    GDALClose(hDataset);
}

/**
 * @brief A partir de una banda especifica escribe una parte (banda) de una imagen .tif.
 * @param hBand Estructura de datos de la banda especificada.
 * @param xSize Tamaño en alto de la imagen.
 * @param ySize Tamaño en ancho de la imagen.
 * @param arrayBand 
*/
void saveBand(GDALRasterBandH hBand, int xSize, int ySize, uint8_t *arrayBand){
    CPLErr error = GDALRasterIO(hBand, GF_Write, 0, 0, xSize, ySize, arrayBand, xSize, ySize, GDT_Byte, 0, 0);
    if (error != CE_None)
    {
        perror("Error: No se pudo guardar la banda en saveBand()");
        exit(EXIT_FAILURE);
    }
}