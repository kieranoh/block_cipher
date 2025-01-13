#include <stdio.h>
#include <stdint.h>
#include <openssl/aes.h>
#include <string.h>
#include <stdlib.h>
#include "bmp_utils.h"


void decrypt_pixel_data_ecb(BMPData *bmpData, const uint8_t *key) {
     
    if (!bmpData || !bmpData->pixelData || bmpData->pixelDataSize == 0) {
        fprintf(stderr, "Invalid BMPData for encryption\n");
        return;
    }

    AES_KEY aes_key;
    AES_set_decrypt_key(key, 128, &aes_key);

        uint8_t block[16];
    uint8_t decrypted_block[16];

    for (size_t i = 0; i <bmpData->pixelDataSize; i += 16) {
        memset(block, 0, 16); // init for padding
        size_t block_size = (i + 16 <= bmpData->pixelDataSize) ? 16 : (bmpData->pixelDataSize - i);
       memcpy(block, bmpData->pixelData + i, block_size); // copy current block


        // AES ECB decrypted
        AES_decrypt(block, decrypted_block, &aes_key);
   
        memcpy(bmpData->pixelData + i, decrypted_block, block_size);
    }

    printf("Finished decryption.\n");
}

int main() {
    const char *encrypted_bmp = "output.bmp";
    const char *decrypted_bmp = "decrypted.bmp";
    uint8_t key[16] = "1234567890abcde"; 

   BMPData *bmpData = read_bmp_to_memory(encrypted_bmp);
    if (!bmpData) {
        printf("Failed to read BMP file\n");
        return 1;
    }

    decrypt_pixel_data_ecb(bmpData, key);

    write_bmp_from_memory(bmpData, decrypted_bmp);

    // Free memory
    free_bmp_data(bmpData);

    printf("BMP encryption and saving complete. Output: %s\n", decrypted_bmp);

    return 0;
}
