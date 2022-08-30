
#include "RC4.h"

/*
 * PRIVATE DEFINITIONS
 */

/*
 * PRIVATE TYPES
 */

/*
 * PRIVATE PROTOTYPES
 */

static inline void RC4_Swap(uint8_t * a, uint8_t * b);

/*
 * PRIVATE VARIABLES
 */

/*
 * PUBLIC FUNCTIONS
 */

void RC4_Init(RC4_t * rc4, const uint8_t * key, uint32_t key_size)
{
	rc4->i1 = 0;
	rc4->i2 = 0;
	for (uint32_t i = 0; i < 256; i++)
	{
		rc4->p[i] = i;
	}

	// Create permuted data
	uint8_t j = 0;
	for (uint32_t i = 0; i < 256; i++)
	{
		j += rc4->p[i] + key[i % key_size];
		RC4_Swap(&rc4->p[i], &rc4->p[j]);
	}
}

void RC4_Encrypt(RC4_t * rc4, const uint8_t * src, uint8_t * dst, uint32_t size)
{
	while(size--)
	{
		// Update permutation stream
		rc4->i1++;
		rc4->i2 += rc4->p[rc4->i1];
		RC4_Swap(&rc4->p[rc4->i1], &rc4->p[rc4->i2]);

		// XOR next byte
		uint8_t j = rc4->p[rc4->i1] + rc4->p[rc4->i2];
		*dst++ = *src++ ^ rc4->p[j];
	}
}

/*
 * PRIVATE FUNCTIONS
 */

static inline void RC4_Swap(uint8_t * a, uint8_t * b)
{
	uint8_t tmp = *a;
	*a = *b;
	*b = tmp;
}

/*
 * INTERRUPT ROUTINES
 */

