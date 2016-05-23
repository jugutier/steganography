#include <openssl/conf.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/obj_mac.h>
#include <string.h>
#include "arguments.h"

/* A 128 bit IV */
unsigned char *iv = (unsigned char *)"0156789012678901";

void crypto_init(void) {
    ERR_load_crypto_strings();
    OpenSSL_add_all_algorithms();
    OPENSSL_config(NULL);
}

void crypto_cleanup(void) {
    EVP_cleanup();
    ERR_free_strings();
}

void crypto_error_handler(void) {
    ERR_print_errors_fp(stderr);
    exit(1);
}

int crypto_get_cipher_nid(CipherAlgorithm algo, CipherMode mode) {
    static int nids[ALGO_MAX][MODE_MAX] = {
        0,
        0,
        0,
        0,
        0,
        0,
        NID_aes_128_ecb,
        NID_aes_128_cfb128,
        NID_aes_128_ofb128,
        NID_aes_128_cbc,
        0,
        NID_aes_192_ecb,
        NID_aes_192_cfb128,
        NID_aes_192_ofb128,
        NID_aes_192_cbc,
        0,
        NID_aes_256_ecb,
        NID_aes_256_cfb128,
        NID_aes_256_ofb128,
        NID_aes_256_cbc,
        0,
        NID_des_ecb,
        NID_des_cfb64,
        NID_des_ofb64,
        NID_des_cbc,
    };

    int nid = nids[algo][mode];
    if (nid == 0) {
        fprintf(stderr, "fatal error: invalid cipher algorithm\n");
        exit(1);
    }
    return nid;
}

int crypto_get_key(char *password, char *key) {
    PKCS5_PBKDF2_HMAC_SHA1(password, strlen(password), NULL, 0, 1000, 32, key);
}

int crypto_encrypt(Encryption *params, char *plaintext, size_t plaintext_len,
                   char *ciphertext) {
    int len, ciphertext_len;

    unsigned char key[64];
    crypto_get_key(params->password, key);

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();

    int nid = crypto_get_cipher_nid(params->algorithm, params->mode);
    EVP_CIPHER *cipher = EVP_get_cipherbynid(nid);

    EVP_EncryptInit_ex(ctx, cipher, NULL, key, iv);
    EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len);
    ciphertext_len = len;

    EVP_EncryptFinal_ex(ctx, ciphertext + len, &len);
    ciphertext_len += len;

    EVP_CIPHER_CTX_free(ctx);

    return ciphertext_len;
}

int crypto_decrypt(Encryption *params, char *ciphertext, size_t ciphertext_len,
                   char *decryptedtext) {
    int len, decryptedtext_len;

    unsigned char key[64];
    crypto_get_key(params->password, key);

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();

    int nid = crypto_get_cipher_nid(params->algorithm, params->mode);
    EVP_CIPHER *cipher = EVP_get_cipherbynid(nid);

    EVP_DecryptInit_ex(ctx, cipher, NULL, key, iv);

    EVP_DecryptUpdate(ctx, decryptedtext, &len,
                      (const unsigned char *)ciphertext, ciphertext_len);
    decryptedtext_len = len;

    EVP_DecryptFinal_ex(ctx, (unsigned char *)decryptedtext + len, &len);
    decryptedtext_len += len;

    EVP_CIPHER_CTX_free(ctx);

    /* Add a NULL terminator. We are expecting printable text */
    decryptedtext[decryptedtext_len] = '\0';

    return decryptedtext_len;
}

/*
int cryptomain(void) {
    char* plaintext = "The quick brown fox jumps over the lazy dog";
    Encryption params = { .algorithm = AES256, .mode = CBC, .password = "123456"
};

    char ciphertext[256];
    char decryptedtext[256];

    int decryptedtext_len, ciphertext_len;
    int plaintext_len = strlen(plaintext);

    crypto_init();

    ciphertext_len = crypto_encrypt(&params, plaintext, plaintext_len,
ciphertext);
    decryptedtext_len = crypto_decrypt(&params, ciphertext, ciphertext_len,
decryptedtext);

    printf("%s\n", decryptedtext);

    crypto_cleanup();

    return 0;
}
*/