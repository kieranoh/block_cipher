#include <stdio.h>
#include <stdint.h>
#include <openssl/aes.h>
#include <string.h>
#include <stdlib.h>
#include "bmp_utils.h"

void encrypt_pixel_data_ecb(BMPData *bmpData, const uint8_t *key) {
    if (!bmpData || !bmpData->pixelData || bmpData->pixelDataSize == 0) {
        fprintf(stderr, "Invalid BMPData for encryption\n");
        return;
    }

    AES_KEY aes_key;
    AES_set_encrypt_key(key, 128, &aes_key);

    uint8_t block[16];
    uint8_t encrypted_block[16];

    for (size_t i = 0; i < bmpData->pixelDataSize; i += 16) {
        memset(block, 0, 16); // Initialize block for padding
        size_t block_size = (i + 16 <= bmpData->pixelDataSize) ? 16 : (bmpData->pixelDataSize - i);
        memcpy(block, bmpData->pixelData + i, block_size); // Copy current block

        // Perform AES encryption in ECB mode
        AES_encrypt(block, encrypted_block, &aes_key);

        // Replace original pixel data with encrypted data
        memcpy(bmpData->pixelData + i, encrypted_block, block_size);
    }

    printf("Finished encryption.\n");
}

int main() {
    const char *bmp_file = "input.bmp";
    const char *output_bmp = "ecb_result/output.bmp";
    uint8_t key[16] = "1234567890abcde"; 

    
    BMPData *bmpData = read_bmp_to_memory(bmp_file);
    if (!bmpData) {
        printf("Failed to read BMP file\n");
        return 1;
    }

    encrypt_pixel_data_ecb(bmpData, key);

    // Write modified BMP data back to a new file
    write_bmp_from_memory(bmpData, output_bmp);

    // Free memory
    free_bmp_data(bmpData);

    printf("BMP encryption and saving complete. Output: %s\n", output_bmp);

    return 0;
}
