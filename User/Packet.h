#ifndef PACKET_H
#define PACKET_H

#include <stdint.h>

/*
 * PUBLIC DEFINITIONS
 */

#define PACKET_HEADER_SIZE			12
#define PACKET_SIZE(payload_size)	(payload_size + PACKET_HEADER_SIZE)

// Packet format:
//   [0   :3   ] Nonce (plaintext)
//   [4   :7   ] Device id
//   [7   :7+n ] Data
//   [8+n :12+n] Nonce

/*
 * PUBLIC TYPES
 */

/*
 * PUBLIC FUNCTIONS
 */

// Returns bytes written to dst
uint32_t Packet_Write(uint8_t * dst, uint32_t device_id, const uint8_t * payload, uint32_t payload_size);

// Returns bytes written to payload
uint32_t Packet_Read(const uint8_t * src, uint32_t src_size, uint32_t * device_id, uint8_t * payload);

/*
 * EXTERN DECLARATIONS
 */

#endif //PACKET_H
