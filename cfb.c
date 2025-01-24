#include <stdio.h>
#include <stdint.h>
#include <openssl/aes.h>
#include <string.h>
#include <stdlib.h>
#include "bmp_utils.h"

void encrypt_pixel_data_cfb(BMPData *bmpData, const uint8_t *key, const uint8_t *iv) {
    if (!bmpData || !bmpData->pixelData || bmpData->pixelDataSize == 0) {
        fprintf(stderr, "Invalid BMPData for encryption\n");
        return;
    }

    AES_KEY aes_key;
    if (AES_set_encrypt_key(key, 128, &aes_key) < 0) {
        fprintf(stderr, "Failed to set encryption key\n");
        return;
    }

    uint8_t feedback[16];      // Feedback register (initially set to IV)
    uint8_t encrypted_block[16]; // Encrypted feedback
    uint8_t block[16];         // Current plaintext block
    memcpy(feedback, iv, 16);  // Initialize feedback with IV

    for (size_t i = 0; i < bmpData->pixelDataSize; i += 16) {
        memset(block, 0, 16); // Initialize block for padding
        size_t block_size = (i + 16 <= bmpData->pixelDataSize) ? 16 : (bmpData->pixelDataSize - i);
        memcpy(block, bmpData->pixelData + i, block_size); // Copy current plaintext block

        // Encrypt feedback using AES
        AES_encrypt(feedback, encrypted_block, &aes_key);

        // XOR encrypted feedback with plaintext to create ciphertext
        for (size_t j = 0; j < block_size; j++) {
            bmpData->pixelData[i + j] = block[j] ^ encrypted_block[j];
        }

        // Update feedback with the resulting ciphertext (CFB chaining)
        memcpy(feedback, bmpData->pixelData + i, 16);
    }

    printf("Finished CFB encryption.\n");
}

int main() {
    const char *bmp_file = "input.bmp";
    const char *output_bmp = "cfb_result/output.bmp";
    uint8_t key[16] = "1234567890abcde";
    uint8_t iv[16] = {0x21, 0x22, 0x23, 0x24, 0x25, 0x24, 0x23, 0x22, 
                      0x21, 0x31, 0x32, 0x33, 0x34, 0x35, 0x34, 0x33};

    BMPData *bmpData = read_bmp_to_memory(bmp_file);
    if (!bmpData) {
        printf("Failed to read BMP file\n");
        return 1;
    }

    encrypt_pixel_data_cfb(bmpData, key, iv);

    // Write modified BMP data back to a new file
    write_bmp_from_memory(bmpData, output_bmp);

    // Free memory
    free_bmp_data(bmpData);

    printf("BMP encryption and saving complete. Output: %s\n", output_bmp);

    return 0;
}
