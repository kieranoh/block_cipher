#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/aes.h>
#include "bmp_utils.h"

void decrypt_pixel_data_pcbc(BMPData *bmpData, const uint8_t *key, const uint8_t *iv) {
    if (!bmpData || !bmpData->pixelData || bmpData->pixelDataSize == 0) {
        fprintf(stderr, "Invalid BMPData for decryption\n");
        return;
    }

    AES_KEY aes_key;
    AES_set_decrypt_key(key, 128, &aes_key);

    uint8_t block[16];
    uint8_t decrypted_block[16];
    uint8_t current_iv[16];
    uint8_t previous_plain_block[16] = {0};

    // Initialize IV
    memcpy(current_iv, iv, 16);

    for (size_t i = 0; i < bmpData->pixelDataSize; i += 16) {
        memset(block, 0, 16); // Initialize block for padding
        size_t block_size = (i + 16 <= bmpData->pixelDataSize) ? 16 : (bmpData->pixelDataSize - i);
        memcpy(block, bmpData->pixelData + i, block_size); // Copy current ciphertext block

        // Decrypt the ciphertext block
        AES_decrypt(block, decrypted_block, &aes_key);

        // XOR the decrypted block with the current IV and previous plaintext block
        for (size_t j = 0; j < block_size; j++) {
            uint8_t temp = block[j];                // Save C_i (current ciphertext block)
            decrypted_block[j] ^= current_iv[j];    // XOR with C_{i-1}
            decrypted_block[j] ^= previous_plain_block[j]; // XOR with P_{i-1}
            previous_plain_block[j] = decrypted_block[j];  // Update P_i
            current_iv[j] = temp;                   // Update C_i
        }

        // Replace original pixel data with decrypted data
        memcpy(bmpData->pixelData + i, decrypted_block, block_size);
    }

    printf("Finished PCBC decryption.\n");
}


int main() {
    const char *encrypted_bmp = "pcbc_result/output.bmp";
    const char *decrypted_bmp = "pcbc_result/decrypted.bmp";
    uint8_t key[16] = "1234567890abcde"; 
    uint8_t iv[16] = {0x21, 0x22, 0x23, 0x24, 0x25, 0x24, 0x23, 0x22, 
                      0x21, 0x31, 0x32, 0x33, 0x34, 0x35, 0x34, 0x33};

    BMPData *bmpData = read_bmp_to_memory(encrypted_bmp);
    if (!bmpData) {
        printf("Failed to read encrypted BMP file\n");
        return 1;
    }

    decrypt_pixel_data_pcbc(bmpData, key, iv);

    // Write the decrypted BMP data back to a new file
    write_bmp_from_memory(bmpData, decrypted_bmp);

    // Free memory
    free_bmp_data(bmpData);

    printf("BMP decryption and saving complete. Output: %s\n", decrypted_bmp);

    return 0;
}
