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

- Characteristics:
  - Identical plaintext blocks produce identical ciphertext blocks.
  - This can expose patterns in the data, making it less secure for highly repetitive inputs like images.
- Disadvantage:
  - Patterns in the original image may still be visible in the encrypted output due to the deterministic nature of ECB.

### CBC - Cipher Block Chaining

- Each plaintext block is XORed with the previous ciphertext block before encryption.
- The first block uses an Initialization Vector (IV).
- Patterns are hidden, even for repetitive plaintext.

**Advantages**:
- Hides patterns in plaintext.
- More secure than ECB.

**Disadvantages**:
- Errors propagate to subsequent blocks during decryption.
- Requires IV synchronization.