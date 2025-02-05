# block_cipher
This repository contains a C program that encrypts an image using several modes of operation, leveraging OpenSSL for encryption.

## Prerequisites

To run this program on a Linux system, you need to have OpenSSL installed. You can install it using the following commands:

```bash
sudo apt update
sudo apt install openssl libssl-dev
```

## Mode of Operation

### 1. ECB - Electronic Codebook

The **ECB (Electronic Codebook)** mode encrypts each block of data independently using the same encryption key.

- **Characteristics:**
  - Identical plaintext blocks produce identical ciphertext blocks.
  - This can expose patterns in the data, making it less secure for highly repetitive inputs like images.
- **Disadvantage:**
  - Patterns in the original image may still be visible in the encrypted output due to the deterministic nature of ECB.

### 2. CBC - Cipher Block Chaining

The **CBC (Cipher Block Chaining)** mode encrypts each plaintext block by combining it with the ciphertext of the previous block before encryption.

- **Characteristics:**
  - Requires an Initialization Vector (IV) for the first block.
  - Each ciphertext block depends on all previous plaintext blocks.
- **Advantage:**
  - Patterns in plaintext are hidden better compared to ECB.
  - Suitable for encrypting data with repeating patterns.
- **Disadvantage:**
  - Encryption cannot be parallelized due to block dependencies.
  - A corrupted ciphertext block affects decryption of subsequent blocks.


### 3. CTR - Counter Mode

The **CTR (Counter Mode)** mode encrypts plaintext blocks by combining them with the encryption of a counter value, which is incremented for each block.

- **Characteristics:**
  - Converts a block cipher into a stream cipher.
  - The counter ensures each block has a unique input, avoiding repetition in the ciphertext.
  - Parallelizable, allowing faster encryption and decryption.
- **Advantage:**
  - No identical ciphertext blocks for identical plaintext blocks.
  - Ideal for high-performance and random-access encryption.
- **Disadvantage:**
  - The counter must never repeat for the same key, as it compromises security.

### 4. OFB - Output Feedback

The **OFB (Output Feedback)** mode generates a keystream by repeatedly encrypting the IV, which is then XORed with the plaintext blocks to produce ciphertext.

- **Characteristics:**
  - Converts a block cipher into a stream cipher.
  - The IV is encrypted to generate the keystream; plaintext is never directly encrypted.
- **Advantage:**
  - Errors in ciphertext do not propagate to other blocks.
  - Ideal for use in noisy channels.
- **Disadvantage:**
  - A repeated IV with the same key compromises security.

### 5. CFB - Cipher Feedback

The **CFB (Cipher Feedback)** mode encrypts plaintext by XORing it with the encrypted output of the previous ciphertext block (or the IV for the first block).

- **Characteristics:**
  - Converts a block cipher into a self-synchronizing stream cipher.
  - The ciphertext of one block is fed back into the encryption process for the next block.
- **Advantage:**
  - Errors in ciphertext are limited to a single block.
  - Can encrypt data smaller than the block size.
- **Disadvantage:**
  - Encryption cannot be parallelized.
  - Requires careful handling of the IV to avoid vulnerabilities.

### 6. PCBC - Propagating Cipher Block Chaining

The **PCBC (Propagating Cipher Block Chaining)** mode encrypts plaintext blocks by XORing them with both the previous plaintext block and the previous ciphertext block before encryption.

- **Characteristics:**
  - Enhances error propagation by making each ciphertext block depend on both the previous plaintext and ciphertext blocks.
  - Requires an Initialization Vector (IV) for the first block.
- **Advantage:**
  - Provides additional error propagation, making it useful in scenarios where tampering detection is important.
- **Disadvantage:**
  - Errors in ciphertext affect all subsequent blocks during decryption.
  - Rarely used due to lack of standardization and support.
