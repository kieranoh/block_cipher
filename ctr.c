#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/aes.h>
#include "bmp_utils.h"

void process_pixel_data_ctr(BMPData *bmpData, const uint8_t *key, const uint8_t *nonce) {
    if (!bmpData || !bmpData->pixelData || bmpData->pixelDataSize == 0) {
        fprintf(stderr, "Invalid BMPData for encryption/decryption\n");
        return;
    }

    AES_KEY aes_key;
    AES_set_encrypt_key(key, 128, &aes_key);

    uint8_t counter[16];
    uint8_t keystream_block[16];
    uint8_t block[16];

    // Initialize counter with nonce (first 8 bytes nonce, last 8 bytes counter starting at 0)
    memset(counter, 0, 16);
    memcpy(counter, nonce, 8);

    for (size_t i = 0; i < bmpData->pixelDataSize; i += 16) {
        // Encrypt counter to generate keystream block
        AES_encrypt(counter, keystream_block, &aes_key);

        // XOR the plaintext block with the keystream block
        size_t block_size = (i + 16 <= bmpData->pixelDataSize) ? 16 : (bmpData->pixelDataSize - i);
        memcpy(block, bmpData->pixelData + i, block_size);
        for (size_t j = 0; j < block_size; j++) {
            block[j] ^= keystream_block[j];
        }

        // Write back the processed block to pixel data
        memcpy(bmpData->pixelData + i, block, block_size);

        // Increment counter (last 8 bytes)
        for (int j = 15; j >= 8; j--) {
            if (++counter[j] != 0) {
                break;
            }
        }
    }

    printf("Finished CTR encryption\n");
}

int main() {
    const char *bmp_file = "input.bmp";
    const char *output_bmp = "ctr_result/output.bmp";
    uint8_t key[16] = "1234567890abcde"; 
    uint8_t nonce[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};

    BMPData *bmpData = read_bmp_to_memory(bmp_file);
    if (!bmpData) {
        printf("Failed to read BMP file\n");
        return 1;
    }

    process_pixel_data_ctr(bmpData, key, nonce);

    // Write the processed BMP data back to a new file
    write_bmp_from_memory(bmpData, output_bmp);

    // Free memory
    free_bmp_data(bmpData);

    printf("BMP encryption/decryption and saving complete. Output: %s\n", output_bmp);

    return 0;
}
