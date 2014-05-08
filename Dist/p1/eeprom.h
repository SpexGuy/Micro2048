#ifndef _EEPROM_H_
#define _EEPROM_H_ 1
#include <stdint.h>
#include "lm4f120h5qr.h"

#define EEPROM_READ_INST 0x3
#define EEPROM_WRITE_INST 0x2
#define EEPROM_READ_SR 0x05
#define EEPROM_WRITE_SR 0x01
#define EEPROM_WRITE_EN 0x06
#define EEPROM_WRITE_DISABLE 0x04
#define EEPROM_SR_WIP 0x01
#define EEPROM_SR_WEL 0x02
#define EEPROM_SR_BP0 0x04
#define EEPROM_SR_BP1 0x08
#define EEPROM_SR_WPEN 0x80

typedef uint16_t EEPROM_ADDR;

//*********Functions***********//

uint8_t spi_eeprom_read_byte(EEPROM_ADDR address);
void spi_eeprom_read_array(EEPROM_ADDR address, uint8_t *data, uint8_t len);
void spi_eeprom_write_byte(EEPROM_ADDR address, uint8_t data);
void spi_eeprom_write_array(EEPROM_ADDR address, uint8_t *data, uint8_t len);
uint8_t spi_eeprom_read_status(void);
void spi_eeprom_write_enable(void);
void spi_eeprom_write_disable(void);
void spi_eeprom_wait_write_in_progress(void);

//********* Structs ***********//

typedef struct {
	uint8_t inst;
	uint8_t addr_hi;
	uint8_t addr_low;
	uint8_t data;
} SPI_EEPROM_BYTE_CMD;

typedef struct {
	uint8_t inst;
	uint8_t data;
} SPI_EEPROM_CFG_CMD;

typedef struct {
	uint8_t inst;
} SPI_EEPROM_EN_CMD;

typedef struct {
	uint8_t inst;
	uint8_t addr_hi;
	uint8_t addr_low;
	uint8_t data[1]; //allocate as much memory as you need here
} SPI_EEPROM_EXT_CMD;

#endif
