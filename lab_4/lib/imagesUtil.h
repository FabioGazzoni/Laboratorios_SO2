#include <omp.h>
#include <gdal.h>
#include <gdal/cpl_conv.h>

typedef struct Bands
{
    struct Size
    {
        int x;
        int y;
    } Size;
    
    uint8_t *red;
    uint8_t *green;
    uint8_t *blue;
} Bands;


GDALDatasetH openImageFile(const char *file);
Bands getBandsImage(GDALDatasetH hDataset);
uint8_t *getMatrixBand(GDALRasterBandH hBand, int xSize, int ySize);
Bands filterBands(Bands bands, int threads);
uint8_t *matrixConvolution(uint8_t *matrix, int xSize, int ySize, int threads);
void saveImage(Bands bands);
void saveBand(GDALRasterBandH hBand, int xSize, int ySize, uint8_t *arrayBand);