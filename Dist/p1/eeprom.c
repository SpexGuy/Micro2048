#include "eeprom.h"
#include "spi.h"

#include "stdlib.h"

uint8_t spi_eeprom_read_byte(uint16_t address) {
	uint8_t dataOut;
	SPI_EEPROM_BYTE_CMD send_packet;
	SPI_EEPROM_BYTE_CMD recv_packet;
	send_packet.inst = EEPROM_READ_INST;
	send_packet.addr_hi = (address >> 8) & 0x00FF;
	send_packet.addr_low = address & 0x00FF;
	send_packet.data = 0;
	spi_eeprom_wait_write_in_progress();
	spiTx((uint8_t *)&send_packet, sizeof(send_packet), (uint8_t *)&recv_packet);
	dataOut = recv_packet.data;
	return dataOut;
}

void spi_eeprom_write_byte(uint16_t address, uint8_t data) {
	SPI_EEPROM_BYTE_CMD send_packet;
	SPI_EEPROM_BYTE_CMD recv_packet;
	spi_eeprom_write_enable();
	send_packet.inst = EEPROM_WRITE_INST;
	send_packet.addr_hi = (address >> 8) & 0x00FF;
	send_packet.addr_low = address & 0x00FF;
	send_packet.data = data;
	spiTx((uint8_t *)&send_packet, sizeof(send_packet), (uint8_t *)&recv_packet);
	spi_eeprom_write_disable();
}

uint8_t spi_eeprom_read_status(void) {
	SPI_EEPROM_CFG_CMD cfg_packet;
	cfg_packet.inst = EEPROM_READ_SR;
	spiTx((uint8_t *)&cfg_packet, sizeof(cfg_packet), (uint8_t *)&cfg_packet);
	return cfg_packet.data;
}

void spi_eeprom_write_enable(void) {
	SPI_EEPROM_EN_CMD en_packet;
	en_packet.inst = EEPROM_WRITE_EN;
	spiTx((uint8_t *)&en_packet, sizeof(en_packet), (uint8_t *)&en_packet);
}

void spi_eeprom_write_disable(void) {
	SPI_EEPROM_EN_CMD en_packet;
	en_packet.inst = EEPROM_WRITE_DISABLE;
	spiTx((uint8_t *)&en_packet, sizeof(en_packet), (uint8_t *)&en_packet);
}

void spi_eeprom_wait_write_in_progress(void) {
	uint8_t status;
	do {
		status = spi_eeprom_read_status();
	} while(status & EEPROM_SR_WIP);
}

