
#include "Packet.h"
#include "RC4.h"
#include <stdlib.h>
#include <string.h>

/*
 * PRIVATE DEFINITIONS
 */

/*
 * PRIVATE TYPES
 */

/*
 * PRIVATE PROTOTYPES
 */

void Packet_InitRC4(RC4_t * rc4, uint32_t nonce);

/*
 * PRIVATE VARIABLES
 */

static const uint8_t PACKET_KEY[16] = {
		0x06, 0xa1, 0x17, 0x32,
		0x09, 0x7e, 0xba, 0x84,
		0x8b, 0x3d, 0x89, 0xd0,
		0x0f, 0xe6, 0x10, 0x11,
};

/*
 * PUBLIC FUNCTIONS
 */

// Returns bytes written to dst
uint32_t Packet_Write(uint8_t * dst, uint32_t device_id, const uint8_t * payload, uint32_t payload_size)
{
	// Get a random number to protect out key from attacks.
	uint32_t nonce = rand();

	RC4_t rc4;
	Packet_InitRC4(&rc4, nonce);

	// Nonce is in the first 4 bytes as plaintext
	memcpy(dst, (uint8_t*)&nonce, 4);

	// Put in the remaining fields
	RC4_Encrypt(&rc4, (uint8_t*)&device_id, dst+4, 					4);
	RC4_Encrypt(&rc4, payload, 				dst+8, 					payload_size);

	// Nonce is in the last 4 bytes - can be used to verify decryption
	RC4_Encrypt(&rc4, (uint8_t*)&nonce, 	dst+8+payload_size,		4);

	return payload_size + PACKET_HEADER_SIZE;
}

// Returns bytes written to payload
uint32_t Packet_Read(const uint8_t * src, uint32_t src_size, uint32_t * device_id, uint8_t * payload)
{
	if (src_size < PACKET_HEADER_SIZE)
	{
		return 0;
	}

	uint32_t payload_size = src_size - PACKET_HEADER_SIZE;

	// Nonce is in plaintext at front of packet
	uint32_t nonce;
	uint32_t nonce2;
	memcpy((uint8_t*)&nonce, src+0, 4);

	RC4_t rc4;
	Packet_InitRC4(&rc4, nonce);

	RC4_Encrypt(&rc4, src+4, 				(uint8_t*)device_id, 		4);
	RC4_Encrypt(&rc4, src+8, 				payload, 					payload_size);
	RC4_Encrypt(&rc4, src+8+payload_size, 	(uint8_t*)&nonce2,			4);

	if (nonce != nonce2)
	{
		return 0;
	}

	return payload_size;
}

/*
 * PRIVATE FUNCTIONS
 */

void Packet_InitRC4(RC4_t * rc4, uint32_t nonce)
{
	// Create a temporary key from our nonce + PSK.
	uint8_t temp_key[sizeof(PACKET_KEY) + 4];
	memcpy(temp_key, (uint8_t*)&nonce, 4);
	memcpy(temp_key+4, PACKET_KEY, sizeof(PACKET_KEY));
	RC4_Init(rc4, temp_key, sizeof(temp_key));
}

/*
 * INTERRUPT ROUTINES
 */

