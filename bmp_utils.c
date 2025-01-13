#include "bmp_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/aes.h>

void save_bmp_to_txt(const char *bmp_file, const char *txt_file) {
    FILE *bmp = fopen(bmp_file, "rb");
    if (!bmp) {
        perror("can not open BMP file!");
        return;
    }

    FILE *txt = fopen(txt_file, "w");
    if (!txt) {
        perror("can not open txt file!");
        fclose(bmp);
        return;
    }

    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER infoHeader;

    // read BMP file header 
    fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, bmp);
    fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, bmp);

    // save as txt file
    fprintf(txt, "bfType:%u\nbfSize:%u\nbfOffBits:%u\n", fileHeader.bfType, fileHeader.bfSize, fileHeader.bfOffBits);
    fprintf(txt, "biWidth:%d\nbiHeight:%d\nbiBitCount:%d\n", infoHeader.biWidth, infoHeader.biHeight, infoHeader.biBitCount);

    // save pixel data
    uint32_t pixel_data_size = fileHeader.bfSize - fileHeader.bfOffBits;
    uint8_t *pixel_data = (uint8_t *)malloc(pixel_data_size);
    if (!pixel_data) {
        perror("memmory allocate fail");
        fclose(bmp);
        fclose(txt);
        return;
    }

    fseek(bmp, fileHeader.bfOffBits, SEEK_SET);
    fread(pixel_data, pixel_data_size, 1, bmp);

    fprintf(txt, "PixelData:");
    for (uint32_t i = 0; i < pixel_data_size; i++) {
        fprintf(txt, "%02x", pixel_data[i]);
        if (i % 16 == 15) fprintf(txt, "\n"); //easy to read
    }

    free(pixel_data);
    fclose(bmp);
    fclose(txt);

    printf("finish save BMP file as txt : %s\n", txt_file);
}

void load_txt_to_bmp(const char *txt_file, const char *bmp_file) {
    FILE *txt = fopen(txt_file, "r");
    if (!txt) {
        perror("can not open txt fike");
        return;
    }

    FILE *bmp = fopen(bmp_file, "wb");
    if (!bmp) {
        perror("can not open BMP file");
        fclose(txt);
        return;
    }

    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER infoHeader;

    // read header info
    fscanf(txt, "bfType:%hu\nbfSize:%u\nbfOffBits:%u\n", &fileHeader.bfType, &fileHeader.bfSize, &fileHeader.bfOffBits);
    fscanf(txt, "biWidth:%d\nbiHeight:%d\nbiBitCount:%hu\n", &infoHeader.biWidth, &infoHeader.biHeight, &infoHeader.biBitCount);

    fileHeader.bfReserved1 = 0;
    fileHeader.bfReserved2 = 0;

    infoHeader.biPlanes = 1;
    infoHeader.biCompression = 0;
    infoHeader.biSize = sizeof(BITMAPINFOHEADER);
    infoHeader.biSizeImage = fileHeader.bfSize - fileHeader.bfOffBits;
    infoHeader.biXPelsPerMeter = 0;
    infoHeader.biYPelsPerMeter = 0;
    infoHeader.biClrUsed = 0;
    infoHeader.biClrImportant = 0;

    // raad pixel data
    uint32_t pixel_data_size = infoHeader.biSizeImage;
    uint8_t *pixel_data = (uint8_t *)malloc(pixel_data_size);
    if (!pixel_data) {
        perror("memmory allocate fail");
        fclose(txt);
        fclose(bmp);
        return;
    }

    fseek(txt, strlen("PixelData:"), SEEK_CUR);
    for (uint32_t i = 0; i < pixel_data_size; i++) {
        unsigned int byte;
        fscanf(txt, "%02x", &byte);
        pixel_data[i] = (uint8_t)byte;
    }

    // write bmp file
    fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, bmp);
    fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, bmp);
    fwrite(pixel_data, pixel_data_size, 1, bmp);

    free(pixel_data);
    fclose(txt);
    fclose(bmp);

    printf("finish save txt file as BMP: %s\n", bmp_file);
}

uint8_t *read_pixel_data_from_txt(const char *txt_file, size_t *data_size) {
    FILE *file = fopen(txt_file, "r");
    if (!file) {
        perror("can not open file");
        return NULL;
    }

    // calculate file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    // read file
    char *buffer = (char *)malloc(file_size + 1);
    if (!buffer) {
        perror("memmory allocate fail");
        fclose(file);
        return NULL;
    }
    fread(buffer, 1, file_size, file);
    buffer[file_size] = '\0';

    // "PixelData:" find keyword
    char *pixel_data_start = strstr(buffer, "PixelData:");
    if (!pixel_data_start) {
        printf("PixelData: cannot find keyword\n");
        free(buffer);
        fclose(file);
        return NULL;
    }

    // "PixelData:" get data after keyword
    pixel_data_start += strlen("PixelData:");
    size_t buffer_size = (file_size - (pixel_data_start - buffer));

    // change binay data
    uint8_t *pixel_data = (uint8_t *)malloc(buffer_size / 2);
    if (!pixel_data) {
        perror("PixelData memmory allocate fail");
        free(buffer);
        fclose(file);
        return NULL;
    }

    size_t pixel_index = 0;
    char *line = strtok(pixel_data_start, "\n");
    while (line) {
        char *ptr = line;
        while (*ptr) {
            unsigned int byte;
            if (sscanf(ptr, "%02x", &byte) == 1) {
                pixel_data[pixel_index++] = (uint8_t)byte;
            }
            ptr += 2;
            if (*ptr == ' ') ptr++;
        }
        line = strtok(NULL, "\n");
    }

    free(buffer);
    fclose(file);

    *data_size = pixel_index; // PixelData return size
    return pixel_data;
}

BMPData *read_bmp_to_memory(const char *bmp_file) {
    FILE *bmp = fopen(bmp_file, "rb");
    if (!bmp) {
        perror("Cannot open BMP file");
        return NULL;
    }

    BMPData *bmpData = (BMPData *)malloc(sizeof(BMPData));
    if (!bmpData) {
        perror("Memory allocation failed for BMPData");
        fclose(bmp);
        return NULL;
    }

    // Read file and info headers
    fread(&bmpData->fileHeader, sizeof(BITMAPFILEHEADER), 1, bmp);
    fread(&bmpData->infoHeader, sizeof(BITMAPINFOHEADER), 1, bmp);

    // Calculate pixel data size and allocate memory
    bmpData->pixelDataSize = bmpData->fileHeader.bfSize - bmpData->fileHeader.bfOffBits;
    bmpData->pixelData = (uint8_t *)malloc(bmpData->pixelDataSize);
    if (!bmpData->pixelData) {
        perror("Memory allocation failed for pixel data");
        free(bmpData);
        fclose(bmp);
        return NULL;
    }

    // Read pixel data
    fseek(bmp, bmpData->fileHeader.bfOffBits, SEEK_SET);
    fread(bmpData->pixelData, bmpData->pixelDataSize, 1, bmp);

    fclose(bmp);
    return bmpData;
}

void write_bmp_from_memory(const BMPData *bmpData, const char *output_file) {
    FILE *bmp = fopen(output_file, "wb");
    if (!bmp) {
        perror("Cannot open output BMP file");
        return;
    }

    // Write file and info headers
    fwrite(&bmpData->fileHeader, sizeof(BITMAPFILEHEADER), 1, bmp);
    fwrite(&bmpData->infoHeader, sizeof(BITMAPINFOHEADER), 1, bmp);

    // Write pixel data
    fwrite(bmpData->pixelData, bmpData->pixelDataSize, 1, bmp);

    fclose(bmp);
}

void free_bmp_data(BMPData *bmpData) {
    if (!bmpData) return;
    free(bmpData->pixelData); // Release pixel data
    free(bmpData);            // Release BMPData structure
}
