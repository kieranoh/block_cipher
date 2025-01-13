#ifndef BMP_UTILS_H
#define BMP_UTILS_H

#include <stdint.h>
#include <stddef.h>

// BMP file header structure
#pragma pack(push, 1)
typedef struct {
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
} BITMAPFILEHEADER;

// BMP info header structure
typedef struct {
    uint32_t biSize;
    int32_t biWidth;
    int32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t biXPelsPerMeter;
    int32_t biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
} BITMAPINFOHEADER;

typedef struct {
    BITMAPFILEHEADER fileHeader;  
    BITMAPINFOHEADER infoHeader; 
    uint8_t *pixelData;          
    size_t pixelDataSize;        
} BMPData;

#pragma pack(pop)

// Function prototypes
void save_bmp_to_txt(const char *bmp_file, const char *txt_file);
void load_txt_to_bmp(const char *txt_file, const char *bmp_file);
uint8_t *read_pixel_data_from_txt(const char *txt_file, size_t *data_size);
BMPData *read_bmp_to_memory(const char *bmp_file);
void write_bmp_from_memory(const BMPData *bmpData, const char *output_file);
void free_bmp_data(BMPData *bmpData);
#endif // BMP_UTILS_H
