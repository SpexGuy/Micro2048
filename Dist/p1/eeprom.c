#include "eeprom.h"
#include "spi.h"

#include <stdlib.h>
#include <alloca.h>
#include <string.h>

uint8_t spi_eeprom_read_byte(EEPROM_ADDR address) {
	SPI_EEPROM_BYTE_CMD send_packet;
	SPI_EEPROM_BYTE_CMD recv_packet;
	send_packet.inst = EEPROM_READ_INST;
	send_packet.addr_hi = (address >> 8) & 0x00FF;
	send_packet.addr_low = address & 0x00FF;
	spi_eeprom_wait_write_in_progress();
	spiTx((uint8_t *)&send_packet, sizeof(send_packet), (uint8_t *)&recv_packet);
	return recv_packet.data;
}

void spi_eeprom_read_array(EEPROM_ADDR address, uint8_t *data, uint8_t len) {
	uint8_t size = sizeof(SPI_EEPROM_EXT_CMD) + len - 1;
	SPI_EEPROM_EXT_CMD *send_packets = alloca(size);
	SPI_EEPROM_EXT_CMD *recv_packets = alloca(size);
	send_packets->inst = EEPROM_READ_INST;
	send_packets->addr_hi = (address >> 8) & 0x00FF;
	send_packets->addr_low = address & 0x00FF;
	spi_eeprom_wait_write_in_progress();
	spiTx((uint8_t *)send_packets, size, (uint8_t *)recv_packets);
	memcpy(data, recv_packets->data, len);
}

void spi_eeprom_write_byte(EEPROM_ADDR address, uint8_t data) {
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

void spi_eeprom_write_array(EEPROM_ADDR address, uint8_t *data, uint8_t len) {
	uint8_t size = sizeof(SPI_EEPROM_EXT_CMD) + len - 1;
	SPI_EEPROM_EXT_CMD *send_packets = alloca(size);
	SPI_EEPROM_EXT_CMD *recv_packets = alloca(size);
	spi_eeprom_write_enable();
	send_packets->inst = EEPROM_WRITE_INST;
	send_packets->addr_hi = (address >> 8) & 0x00FF;
	send_packets->addr_low = address & 0x00FF;
	memcpy(send_packets->data, data, len);
	spiTx((uint8_t *)send_packets, size, (uint8_t *)recv_packets);
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

