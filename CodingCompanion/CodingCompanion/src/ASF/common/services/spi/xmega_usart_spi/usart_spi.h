/**
 * \file
 *
 * \brief AVR XMEGA USART in SPI mode driver functions.
 *
 * Copyright (c) 2010-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#ifndef _USART_SPI_H_
#define _USART_SPI_H_

#include "compiler.h"
#include "status_codes.h"
#include "ioport.h"
#include "usart.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! \name USART in SPI mode Management Configuration
 */
//! @{
#include "conf_usart_spi.h"

//! Default Config Spi Master Dummy Field
#ifndef CONFIG_USART_SPI_DUMMY
	#define CONFIG_USART_SPI_DUMMY              0xFF
#endif
//! @}

/**
 * \brief Clock phase
 */
#define SPI_CPHA   (1 << 0)

/**
 * \brief Clock polarity
 */
#define SPI_CPOL   (1 << 1)

/**
 * \brief SPI mode 0
 */
#define SPI_MODE_0  0
/**
 * \brief SPI mode 1
 */
#define SPI_MODE_1  (SPI_CPHA)
/**
 * \brief SPI mode 2
 */
#define SPI_MODE_2  (SPI_CPOL)
/**
 * \brief SPI mode 3
 */
#define SPI_MODE_3  (SPI_CPOL | SPI_CPHA)

typedef uint8_t spi_flags_t;
typedef uint32_t board_spi_select_id_t;

//! \brief Polled SPI device definition
struct usart_spi_device {
	//! Board specific select id
	port_pin_t id;
};

/*! \brief Initializes the USART in SPI master mode.
 *
 * \param usart Base address of the USART instance.
 *
 */
extern void usart_spi_init(USART_t *usart);

/**
 * \brief Setup a USART in SPI mode device.
 *
 * The returned device descriptor structure must be passed to the driver
 * whenever that device should be used as current slave device.
 *
 * \param usart     Base address of the USART instance.
 * \param device    Pointer to usart device struct that should be initialized.
 * \param flags     USART configuration flags. Common flags for all
 *                  implementations are the usart modes SPI_MODE_0 ...
 *                  SPI_MODE_3.
 * \param baud_rate Baud rate for communication with slave device in Hz.
 * \param sel_id    Board specific select id
 */
extern void usart_spi_setup_device(USART_t *usart, struct usart_spi_device *device,
		spi_flags_t flags, unsigned long baud_rate,
		board_spi_select_id_t sel_id);

/*! \brief Enables the USART for the specified USART in SPI mode.
 *
 * \param usart Base address of the USART instance.
 */
extern void usart_spi_enable(USART_t *usart);

/*! \brief Disables the USART.
 *
 * Ensures that nothing is transferred while setting up buffers.
 *
 * \param usart Base address of the USART instance.
 *
 */
extern void usart_spi_disable(USART_t *usart);


/*! \brief Tests if the USART in SPI mode is enabled.
 *
 * \param usart Base address of the usart instance.
 *
 * \return \c 1 if the usart is enabled, otherwise \c 0.
 */
extern bool usart_spi_is_enabled(USART_t *usart);

/**
 * \brief Select given device on the SPI bus
 *
 * Set device specific setting and calls board chip select.
 *
 * \param usart Base address of the USART instance.
 * \param device SPI device
 *
 */
extern void usart_spi_select_device(USART_t *usart, struct usart_spi_device *device);

/**
 * \brief Deselect given device on the SPI bus
 *
 * Calls board chip deselect.
 *
 * \param usart Base address of the USART instance.
 * \param device SPI device
 *
 * \pre SPI device must be selected with usart_spi_select_device() first
 */
extern void usart_spi_deselect_device(USART_t *usart, struct usart_spi_device *device);

/*! \brief Write one byte to a SPI device using USART in SPI mode
 *
 * \param usart Base address of the usart instance.
 * \param data The data byte to be loaded
 *
 */
__always_inline static void usart_spi_write_single(USART_t *usart, uint8_t data)
{
	usart_spi_transmit(usart, data);
}

/**
 * \brief Send a sequence of bytes to a SPI device using USART in SPI mode
 *
 * Received bytes on the USART in SPI mode are discarded.
 *
 * \param usart Base address of the USART instance.
 * \param data  Data buffer to write
 * \param len   Length of data
 *
 * \pre usart device must be selected with usart_spi_select_device() first
 */
extern status_code_t usart_spi_write_packet(USART_t *usart,const uint8_t *data, size_t len);


/*! \brief Receive one byte from a SPI device using USART in SPI mode
 *
 * \param usart Base address of the USART instance.
 * \param data Pointer to the data byte where to store the received data.
 *
 */
inline static void usart_spi_read_single(USART_t *usart, uint8_t *data)
{
	*data = usart_spi_transmit(usart, CONFIG_USART_SPI_DUMMY);
}

/**
 * \brief Receive a sequence of bytes from a USART in SPI mode device
 *
 * All bytes sent out on usart bus are sent as value 0.
 *
 * \param usart Base address of the usart instance.
 * \param data   data buffer to read
 * \param len    Length of data
 *
 * \pre usart device must be selected with usart_spi_select_device() first
 */
extern status_code_t usart_spi_read_packet(USART_t *usart, uint8_t *data, size_t len);

/*! \brief Check whether there are data in Transmit Holding Register or
 *         Transmit Shift Register in SPI master mode.
 *
 * \param usart Base address of the USART instance.
 *
 * \retval 1      The two registers are empty.
 * \retval 0      One of the two registers contains data.
 */
inline static bool usart_spi_is_tx_empty(USART_t *usart)
{
	return usart_data_register_is_empty(usart);
}

/*! \brief Check whether the USART in SPI master mode contains a received character.
 *
 * \param usart Base address of the USART instance.
 *
 * \retval 1      Some data have been received.
 * \retval 0      No data has been received.
 */
inline static bool usart_spi_is_rx_ready(USART_t *usart)
{
	return usart_rx_is_complete(usart);
}

/*! \brief Check if the USART Transmit Register is empty.
 *
 * \param usart Base address of the USART instance.
 *
 * \retval 1      There is no data in the Transmit Holding Register.
 * \retval 0      There are data in the Transmit Holding Register.
 */
inline static bool usart_spi_is_tx_ready(USART_t *usart)
{
	return usart_data_register_is_empty(usart);
}

/*! \brief Tests if the USART in SPI mode contains a received character.
 *
 * \param usart Base address of the USART instance.
 *
 * \return \c 1 if the USART Receive Holding Register is full, otherwise \c 0.
 */
inline static bool usart_spi_is_rx_full(USART_t *usart)
{
	return usart_rx_is_complete(usart);
}

#ifdef __cplusplus
}
#endif

#endif  // _USART_SPI_H_
