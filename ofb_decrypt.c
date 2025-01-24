#include <stdio.h>
#include <stdint.h>
#include <openssl/aes.h>
#include <string.h>
#include <stdlib.h>
#include "bmp_utils.h"

void decrypt_pixel_data_ofb(BMPData *bmpData, const uint8_t *key, const uint8_t *iv) {
    if (!bmpData || !bmpData->pixelData || bmpData->pixelDataSize == 0) {
        fprintf(stderr, "Invalid BMPData for decryption\n");
        return;
    }

    AES_KEY aes_key;
    if (AES_set_encrypt_key(key, 128, &aes_key) < 0) {
        fprintf(stderr, "Failed to set encryption key\n");
        return;
    }

    uint8_t feedback[16];      // Feedback register (initially set to IV)
    uint8_t encrypted_feedback[16]; // Encrypted feedback
    uint8_t block[16];         // Current ciphertext block
    memcpy(feedback, iv, 16);  // Initialize feedback with IV

    for (size_t i = 0; i < bmpData->pixelDataSize; i += 16) {
        memset(block, 0, 16); // Initialize block for padding
        size_t block_size = (i + 16 <= bmpData->pixelDataSize) ? 16 : (bmpData->pixelDataSize - i);
        memcpy(block, bmpData->pixelData + i, block_size); // Copy current ciphertext block

        // Encrypt feedback to produce key stream
        AES_encrypt(feedback, encrypted_feedback, &aes_key);

        // XOR encrypted feedback (key stream) with ciphertext to recover plaintext
        for (size_t j = 0; j < block_size; j++) {
            bmpData->pixelData[i + j] = block[j] ^ encrypted_feedback[j];
        }

        // Update feedback with the encrypted feedback (OFB chaining)
        memcpy(feedback, encrypted_feedback, 16);
    }

    printf("Finished OFB decryption.\n");
}

int main() {
    const char *encrypted_bmp_file = "ofb_result/output.bmp";
    const char *output_bmp = "ofb_result/decrypted_output.bmp";
    uint8_t key[16] = "1234567890abcde";
    uint8_t iv[16] = {0x21, 0x22, 0x23, 0x24, 0x25, 0x24, 0x23, 0x22, 
                      0x21, 0x31, 0x32, 0x33, 0x34, 0x35, 0x34, 0x33};

    BMPData *bmpData = read_bmp_to_memory(encrypted_bmp_file);
    if (!bmpData) {
        printf("Failed to read encrypted BMP file\n");
        return 1;
    }

    decrypt_pixel_data_ofb(bmpData, key, iv);

    // Write decrypted BMP data back to a new file
    write_bmp_from_memory(bmpData, output_bmp);

    // Free memory
    free_bmp_data(bmpData);

    printf("BMP decryption and saving complete. Output: %s\n", output_bmp);

    return 0;
}
