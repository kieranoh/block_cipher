#include <stdio.h>
#include <stdint.h>
#include <openssl/aes.h>
#include <string.h>
#include <stdlib.h>
#include "bmp_utils.h"

void encrypt_pixel_data_pcbc(BMPData *bmpData, const uint8_t *key, const uint8_t *iv) {
    if (!bmpData || !bmpData->pixelData || bmpData->pixelDataSize == 0) {
        fprintf(stderr, "Invalid BMPData for encryption\n");
        return;
    }

    AES_KEY aes_key;
    if (AES_set_encrypt_key(key, 128, &aes_key) < 0) {
        fprintf(stderr, "Failed to set encryption key\n");
        return;
    }

    uint8_t block[16];
    uint8_t encrypted_block[16];
    uint8_t current_iv[16];
    uint8_t previous_plain_block[16] = {0}; // To store the previous plaintext block

    // Initialize IV
    memcpy(current_iv, iv, 16);

    for (size_t i = 0; i < bmpData->pixelDataSize; i += 16) {
        memset(block, 0, 16); // Initialize block for padding
        size_t block_size = (i + 16 <= bmpData->pixelDataSize) ? 16 : (bmpData->pixelDataSize - i);
        memcpy(block, bmpData->pixelData + i, block_size); // Copy current block

        // Store plaintext block before modification
        uint8_t temp_plain_block[16];
        memcpy(temp_plain_block, block, 16);

        // XOR the block with the current IV and the previous plaintext block (PCBC chaining)
        for (size_t j = 0; j < block_size; j++) {
            block[j] ^= current_iv[j] ^ previous_plain_block[j];
        }

        // Perform AES encryption in PCBC mode
        AES_encrypt(block, encrypted_block, &aes_key);

        // Replace original pixel data with encrypted data
        memcpy(bmpData->pixelData + i, encrypted_block, block_size);

        // Update IV to the encrypted block
        memcpy(current_iv, encrypted_block, 16);

        // Store the original plaintext block for the next iteration
        memcpy(previous_plain_block, temp_plain_block, 16);
    }

    printf("Finished PCBC encryption.\n");
}



int main() {
    const char *bmp_file = "input.bmp";
    const char *output_bmp = "pcbc_result/output.bmp";
    uint8_t key[16] = "1234567890abcde"; 
    uint8_t iv[16] = {0x21, 0x22, 0x23, 0x24, 0x25, 0x24, 0x23, 0x22, 
                      0x21, 0x31, 0x32, 0x33, 0x34, 0x35, 0x34, 0x33};

    BMPData *bmpData = read_bmp_to_memory(bmp_file);
    if (!bmpData) {
        printf("Failed to read BMP file\n");
        return 1;
    }

    encrypt_pixel_data_pcbc(bmpData, key, iv);

    // Write modified BMP data back to a new file
    write_bmp_from_memory(bmpData, output_bmp);

    // Free memory
    free_bmp_data(bmpData);

    printf("BMP encryption and saving complete. Output: %s\n", output_bmp);

    return 0;
}
