#include <stdio.h>
#include <stdint.h>
#include <openssl/aes.h>
#include <string.h>
#include <stdlib.h>

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
#pragma pack(pop)

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

// ECB block mode operation
void encrypt_pixel_data_ecb(uint8_t *data, size_t data_size, const uint8_t *key, const char *output_file, const char *input_file) {
    AES_KEY aes_key;
    AES_set_encrypt_key(key, 128, &aes_key);

    // save data
    FILE *output = fopen(output_file, "wb");
    if (!output) {
        perror("can not open file");
        return;
    }

    FILE *txt = fopen(input_file, "r");
    if (!txt) {
        perror("can not open txt fike");
        return;
    }

    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER infoHeader;

    // read header info
    fscanf(txt, "bfType:%hu\nbfSize:%u\nbfOffBits:%u\n", &fileHeader.bfType, &fileHeader.bfSize, &fileHeader.bfOffBits);
    fscanf(txt, "biWidth:%d\nbiHeight:%d\nbiBitCount:%hu\n", &infoHeader.biWidth, &infoHeader.biHeight, &infoHeader.biBitCount);

    fprintf(output, "bfType:%u\nbfSize:%u\nbfOffBits:%u\n", fileHeader.bfType, fileHeader.bfSize, fileHeader.bfOffBits);
    fprintf(output, "biWidth:%d\nbiHeight:%d\nbiBitCount:%d\n", infoHeader.biWidth, infoHeader.biHeight, infoHeader.biBitCount);
    fprintf(output,"PixelData:");
    fclose(txt);

    uint8_t block[16];
    uint8_t encrypted_block[16];

    for (size_t i = 0; i < data_size; i += 16) {
        memset(block, 0, 16); // init for padding
        size_t block_size = (i + 16 <= data_size) ? 16 : (data_size - i);
        memcpy(block, data + i, block_size); // copy current block


        // AES ECB 
        AES_encrypt(block, encrypted_block, &aes_key);
        for (size_t j = 0; j < 16; j++) {
            fprintf(output, "%02x", encrypted_block[j]); // Write as 2-digit hex
        }
        fprintf(output, "\n"); // Add a newline after each block
            
    }

    fclose(output);
    printf("finish ECB at %s!\n", output_file);
}

int main() {
    const char *bmp_file = "input.bmp";
    const char *txt_file = "image_data.txt";
    const char *output_txt = "ecb_result/output.txt";
    const char *output_bmp = "ecb_result/output.bmp";
    uint8_t key[16] = "1234567890abcde"; 

    // transform BMP as txt
    save_bmp_to_txt(bmp_file, txt_file);

    size_t data_size;
    uint8_t *pixel_data = read_pixel_data_from_txt(txt_file,&data_size);

    encrypt_pixel_data_ecb(pixel_data,data_size,key,output_txt,txt_file);

    // transform txt as BMP
    load_txt_to_bmp(output_txt, output_bmp);

    return 0;
}
