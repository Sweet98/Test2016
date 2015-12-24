#ifndef HABTYPES_H_
#define HABTYPES_H_
#include <stdint.h>
#define APP_BARKER 0x000000b1
//#define DCD_BARKER 0xE91972B1  // this apears to be wrong in documentation
#define DCD_BARKER 0xB17219E9 


typedef struct
{
	uint32_t barker; /* Barker for sanity check */
	uint32_t length; /* Device configuration structure length (not including preamble) */
}
DCD_PREAMBLE_T;

typedef struct
{
	uint32_t type; /* Type of pointer (byte, halfword, word, wait/read) */
	uint32_t *addr; /* Address to write to */
	uint32_t data; /* Data to write */
}
DCD_TYPE_ADDR_DATA_T;

typedef struct
{
	DCD_PREAMBLE_T preamble; /* Preamble */
	/* Type / Address / data elements */
#if COUNT_DCD >0

	DCD_TYPE_ADDR_DATA_T type_addr_data[COUNT_DCD]; /*where count would be some hardcoded value less than 60*/
#endif
}
DCD_T;
typedef struct
{
	uint8_t rsa_exponent[MAX_EXP_SIZE]; /* RSA public exponent */
	uint8_t *rsa_modulus; /* RSA modulus pointer */
	uint16_t exponent_size; /* Exponent size in bytes */
	uint16_t modulus_size; /* Modulus size in bytes */
	uint8_t init_flag; /* Indicates if key initialized */
}
hab_rsa_public_key;
//where:
//uint8_t is an 8 bit unsigned integer
//uint16_t is a 16 bit unsigned integer
//BOOLEAN is an 8 bit flag indicating TRUE or FALSE

typedef struct
{
	uint32_t *app_code_jump_vector;
	uint32_t app_code_barker;
	uint32_t *app_code_csf;
	DCD_T **dcd_ptr_ptr;
	hab_rsa_public_key *super_root_key;
	DCD_T *dcd_ptr;
	uint32_t *app_dest_ptr;
}
FLASH_HDR_T;
//where:
//uint32_t is a 32 bit unsigned integer
//DCD_T is a structure that defines the device configuration table (see below for details)
//hab_rsa_public_key is a structure defines the super root key
//•app_code_jump_vector pointer: Points to the address of the first instruction of

/* Flash Header Structure */
typedef struct
{
	uint32_t length; /* Length of data to be read */
}
FLASH_CFG_PARMS_T;
//where: uint32_t is a 32 bit unsigned integer.
//length: This parameter is required to determine the

#endif /*HABTYPES_H_*/
