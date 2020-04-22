/*********************************************************************
 * Filename:   sha256.h
 * Author:     Brad Conte (brad AT bradconte.com)
 * Copyright:
 * Disclaimer: This code is presented "as is" without any guarantees.
 * Details:    Defines the API for the corresponding SHA1 implementation.
 *********************************************************************/
/* Edited by Chris Feger to use stdint.h types */

#ifndef SHA256_H
#define SHA256_H

#ifdef __cplusplus
extern "C" {
#endif

/*************************** HEADER FILES ***************************/
#include <stddef.h>
#include <stdint.h>

/****************************** MACROS ******************************/
#define SHA256_BLOCK_SIZE 32 // SHA256 outputs a 32 byte digest

/**************************** DATA TYPES ****************************/

typedef struct
{
    uint8_t data[64];
    uint32_t datalen;
    uint64_t bitlen;
    uint32_t state[8];
} SHA256_CTX;

/*********************** FUNCTION DECLARATIONS **********************/
void sha256_init(SHA256_CTX* ctx);
void sha256_update(SHA256_CTX* ctx, const uint8_t data[], size_t len);
void sha256_final(SHA256_CTX* ctx, uint8_t hash[]);
void sha256(uint8_t hash[], uint8_t data[], size_t len);

#ifdef __cplusplus
}
#endif

#endif // SHA256_H
