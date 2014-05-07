#include "eeprom.h"
#include "spi.h"

#include "stdlib.h"

uint8_t spi_eeprom_read_byte(uint16_t address) {
	uint8_t dataOut;
	SPI_EEPROM_BYTE_CMD send_packet = malloc(sizeof(SPI_EEPROM_BYTE_CMD));
	SPI_EEPROM_BYTE_CMD recv_packet = malloc(sizeof(SPI_EEPROM_BYTE_CMD));
	send_packet->inst = EEPROM_READ_INST;
	send_packet->addr_hi = (address >> 8) & 0x00FF;
	send_packet->addr_low = address & 0x00FF;
	send_packet->data = 0;
	spiTx((uint8_t *)send_packet, sizeof(SPI_EEPROM_BYTE_CMD), (uint8_t *)recv_packet);
	dataOut = recv_packet->data;
	free(send_packet);
	free(recv_packet);
	return dataOut;
}

void spi_eeprom_write_byte(uint16_t address, uint8_t data) {
	SPI_EEPROM_BYTE_CMD send_packet = malloc(sizeof(SPI_EEPROM_BYTE_CMD));
	SPI_EEPROM_BYTE_CMD recv_packet = malloc(sizeof(SPI_EEPROM_BYTE_CMD));
	send_packet->inst = EEPROM_WRITE_INST;
	send_packet->addr_hi = (address >> 8) & 0x00FF;
	send_packet->addr_low = address & 0x00FF;
	send_packet->data = data;
	spiTx((uint8_t *)send_packet, sizeof(SPI_EEPROM_BYTE_CMD), (uint8_t *)recv_packet);
	free(send_packet);
	free(recv_packet);
}

uint8_t spi_eeprom_read_status(void) {
	uint8_t dataOut;
	SPI_EEPROM_CFG_CMD cfg_packet = malloc(sizeof(SPI_EEPROM_CFG_CMD));
	cfg_packet->inst = EEPROM_READ_SR;
	spiTx((uint8_t *)cfg_packet, sizeof(SPI_EEPROM_CFG_CMD), (uint8_t *)cfg_packet);
	dataOut = cfg_packet->data;
	free(cfg_packet);
	return dataOut;
}

void spi_eeprom_write_enable(void) {
	SPI_EEPROM_CFG_CMD cfg_packet = malloc(sizeof(SPI_EEPROM_CFG_CMD));
	cfg_packet->inst = EEPROM_WRITE_EN;
	spiTx((uint8_t *)cfg_packet, sizeof(SPI_EEPROM_CFG_CMD), (uint8_t *)cfg_packet);
	free(cfg_packet);
}

void spi_eeprom_write_disable(void) {
	SPI_EEPROM_CFG_CMD cfg_packet = malloc(sizeof(SPI_EEPROM_CFG_CMD));
	cfg_packet->inst = EEPROM_WRITE_DISABLE;
	spiTx((uint8_t *)cfg_packet, sizeof(SPI_EEPROM_CFG_CMD), (uint8_t *)cfg_packet);
	free(cfg_packet);
}

void spi_eeprom_wait_write_in_progress(void) {
	SPI_EEPROM_CFG_CMD cfg_packet = malloc(sizeof(SPI_EEPROM_CFG_CMD));
	cfg_packet->inst = EEPROM_WRITE_SR;
	spiTx((uint8_t *)cfg_packet, sizeof(SPI_EEPROM_CFG_CMD), (uint8_t *)cfg_packet);
}

