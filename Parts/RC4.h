#ifndef RC4_H
#define RC4_H

#include <stdint.h>

/*
 * PUBLIC DEFINITIONS
 */

/*
 * PUBLIC TYPES
 */

typedef struct {
	uint8_t i1;
	uint8_t i2;
	uint8_t p[256];
} RC4_t;

/*
 * PUBLIC FUNCTIONS
 */

// Initialize the RC4 cypher with the given key
void RC4_Init(RC4_t * rc4, const uint8_t * key, uint32_t key_size);

// Note that RC4 Encrypt/Decrypt are the same
void RC4_Encrypt(RC4_t * rc4, const uint8_t * src, uint8_t * dst, uint32_t size);

/*
 * EXTERN DECLARATIONS
 */

#endif //RC4_H
