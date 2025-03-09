#ifndef NRF_H__
#define NRF_H__

/**
 * @file nrf.h
 * Header file for nRF24L01 communication module for Mango Pi.
 * 
 * This file contains function declarations and documentation for configuring 
 * and communicating with the nRF24L01 wireless transceiver using SPI. 
 * The module supports both transmission (TX) and reception (RX) modes, 
 * register manipulation, and data transfer operations.
 * 
 * This module was inspired by ControllerTechs.com's NRF24L01.c module for an STM 32 Microcontroller
 * 
 * Authors: Sahan Samarakoon and Mario Portillo
 * Date: 9th March 2025
 */


#include <stdint.h>
/*
 * Enables the nRF24L01 device by setting the CE pin high.
 */
void enable_nrf_device();

/*
 * Disables the nRF24L01 device by setting the CE pin low.
 */
void disable_nrf_device();

/*
 * Selects the nRF24L01 device by setting the CSN pin low.
 * NOTE: This is handled by spi.h. Therefore, user need not manually
 * select/deselect device for SPI communication.
 */
void select_nrf_device();

/*
 * Deselects the nRF24L01 device by setting the CSN pin high.
 */
void deselect_nrf_device();

/**
 * Writes a single byte to a specified nRF24L01 register.
 *
 * @param reg The register address to write to.
 * @param data The byte value to write to the register.
 */
void nrf24_write_byte_register(uint8_t reg, uint8_t data);

/**
 * Writes multiple bytes to a specified nRF24L01 register.
 *
 * @param reg The register address to write to.
 * @param data Pointer to the data to be written.
 * @param size The number of bytes to write.
 */
void nrf24_write_register(uint8_t reg, uint8_t *data, int size);

/**
 * Reads multiple bytes from a specified nRF24L01 register.
 *
 * @param reg The register address to read from.
 * @param data Pointer to the buffer to store the read data.
 * @param size The number of bytes to read.
 */
void nrf24_read_register(uint8_t reg, uint8_t *data, int size);


/**
 * Sends a single command to the nRF24L01 device.
 *
 * @param cmd The command byte to be sent.
 */
void nrf24_send_cmd(uint8_t cmd);


/*
 * Initializes the nRF24L01 device with default configuration.
 */
void nrf24_init();

/**
 * Configures the nRF24L01 as a transmitter.
 *
 * @param address Pointer to the 5-byte address of the receiver pipe.
 * @param channel The channel number to configure the transmitter on.
 */
void nrf24_set_tx_mode(uint8_t *address, uint8_t channel);  // Configure as a transmitter

/**
 * Transmits a 32-byte payload using the nRF24L01 device.
 *
 * @param data Pointer to the 32-byte payload to be transmitted.
 * @return 1 if the transmission is successful; 0 otherwise.
 */
uint8_t nrf24_transmit(uint8_t *data);

/**
 * Configures the nRF24L01 as a receiver.
 *
 * @param address Pointer to the 5-byte address for RX data pipe 1.
 * @param channel The channel number to configure for receiving data.
 */
void nrf24_set_rx_mode(uint8_t *address, uint8_t channel);


/**
 * Checks if data is available in the RX FIFO for a specific data pipe.
 *
 * @param pipenum The RX data pipe number to check (valid range: 0-5).
 * @return 1 if data is available in the specified data pipe; 0 otherwise.
 */
uint8_t is_data_available(int pipenum);


/**
 * Receives a 32-byte payload from the RX FIFO and copies it to the provided buffer.
 *
 * @param data Pointer to the buffer where the received payload will be stored.
 */
void nrf24_receive(uint8_t *data);



/**
 * Resets specific registers or the entire nRF24L01+ device to default settings.
 *
 * This function resets either individual registers (e.g., STATUS, FIFO_STATUS) or the entire
 * nRF24L01 device based on the provided parameter. To reset the entire device, pass `0` as the 
 * parameter. The reset operation sets registers to their default values as per the datasheet.
 *
 * @param REG The register to reset:
 *            - Pass `STATUS` to reset the STATUS register (default: 0x00).
 *            - Pass `FIFO_STATUS` to reset the FIFO_STATUS register (default: 0x11).
 *            - Pass `0` to reset the entire nRF24L01 device.
 */
void nrf24_reset(uint8_t reg);



enum REGISTERS {
    CONFIG = 0x00,
    EN_AA,
    EN_RXADDR,
    SETUP_AW,
    SETUP_RETR,
    RF_CH,
    RF_SETUP,
    STATUS,
    OBSERVE_TX, 
    CD, 
    RX_ADDR_P0,
    RX_ADDR_P1,
    RX_ADDR_P2,
    RX_ADDR_P3,
    RX_ADDR_P4,
    RX_ADDR_P5,
    TX_ADDR,
    RX_PW_P0,
    RX_PW_P1,
    RX_PW_P2,
    RX_PW_P3,
    RX_PW_P4,
    RX_PW_P5,
    FIFO_STATUS,
    DYNDPD,
    FEATURE,
};

enum COMMANDS{
    R_REGISTER = 0x00,
    W_REGISTER = 0x20,
    REGISTER_MASK = 0x1F,
    ACTIVATE = 0x50,
    R_RX_PL_WID = 0x60,
    R_RX_PAYLOAD = 0x61,
    W_TX_PAYLOAD = 0xA0,
    W_ACK_PAYLOAD = 0xA8,
    FLUSH_TX = 0xE1,
    FLUSH_RX = 0xE2,
    REUSE_TX_PL = 0xE3,
    NOP = 0XFF,
}; 
#endif