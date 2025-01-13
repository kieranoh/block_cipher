#include <stdio.h>
#include <stdint.h>
#include <openssl/aes.h>
#include <string.h>
#include <stdlib.h>
#include "bmp_utils.h"

void decrypt_pixel_data_cbc(BMPData *bmpData, const uint8_t *key, const uint8_t *iv) {
    if (!bmpData || !bmpData->pixelData || bmpData->pixelDataSize == 0) {
        fprintf(stderr, "Invalid BMPData for decryption\n");
        return;
    }

    AES_KEY aes_key;
    AES_set_decrypt_key(key, 128, &aes_key);

    uint8_t block[16];
    uint8_t decrypted_block[16];
    uint8_t current_iv[16];
    uint8_t previous_encrypted_block[16];

    // Initialize IV
    memcpy(current_iv, iv, 16);

    for (size_t i = 0; i < bmpData->pixelDataSize; i += 16) {
        memset(block, 0, 16); // Initialize block for padding
        size_t block_size = (i + 16 <= bmpData->pixelDataSize) ? 16 : (bmpData->pixelDataSize - i);
        memcpy(block, bmpData->pixelData + i, block_size); // Copy current encrypted block

        // Save the current encrypted block for next IV
        memcpy(previous_encrypted_block, block, 16);

        // Perform AES decryption in CBC mode
        AES_decrypt(block, decrypted_block, &aes_key);

        // XOR the decrypted block with the current IV
        for (size_t j = 0; j < block_size; j++) {
            decrypted_block[j] ^= current_iv[j];
        }

        // Replace original pixel data with decrypted data
        memcpy(bmpData->pixelData + i, decrypted_block, block_size);

        // Update IV to the current encrypted block
        memcpy(current_iv, previous_encrypted_block, 16);
    }

    printf("Finished CBC decryption.\n");
}

int main() {
    const char *encrypted_bmp = "cbc_result/output.bmp";
    const char *decrypted_bmp = "cbc_result/decrypted.bmp";
    uint8_t key[16] = "1234567890abcde"; 
    uint8_t iv[16] = {0x21, 0x22, 0x23, 0x24, 0x25, 0x24, 0x23, 0x22, 
                      0x21, 0x31, 0x32, 0x33, 0x34, 0x35, 0x34, 0x33};

    BMPData *bmpData = read_bmp_to_memory(encrypted_bmp);
    if (!bmpData) {
        printf("Failed to read encrypted BMP file\n");
        return 1;
    }

    decrypt_pixel_data_cbc(bmpData, key, iv);

    // Write decrypted BMP data back to a new file
    write_bmp_from_memory(bmpData, decrypted_bmp);

    // Free memory
    free_bmp_data(bmpData);

    printf("BMP decryption and saving complete. Output: %s\n", decrypted_bmp);

    return 0;
}
