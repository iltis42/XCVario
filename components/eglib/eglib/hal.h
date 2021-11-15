#ifndef EGLIB_HAL_H
#define EGLIB_HAL_H

#include "../eglib.h"
#include <stdint.h>
#include <stdbool.h>

/**
 * HAL drivers implement :c:type:`hal_struct` and can accept a configuration
 * (:c:func:`eglib_GetHalConfig`), both which are passed to :c:func:`eglib_Init`.
 */

/**
 * Types
 * =====
 */

/** Type of bytes transmitted */
enum hal_dc_t {
	/** Command */
	HAL_COMMAND,
	/** Data */
	HAL_DATA,
};

/** Endianness */
enum hal_bit_numbering_t {
	/** Big-endian (most significant bit first) */
	HAL_LSB_FIRST,
	/** Little-endian (least significant bit first) */
	HAL_MSB_FIRST,
};

/**
 * I2C Speed.
 *
 * :See also: :c:type:`hal_i2c_config_t`.
 */
enum hal_i2c_speed_t {
	/** 100kHz */
	HAL_I2C_100KHZ,
	/** 400kHz */
	HAL_I2C_400KHZ,
};

/**
 * Display Configuration
 * =====================
 *
 * These are configuration definitions required by the HAL driver :c:type:`hal_struct` and that
 * are provided by display the driver :c:type:`display_struct`.
 */

/**
 * 4-Wire SPI
 * ----------
 */

/**
 * 4-Wire SPI configuration.
 *
 * :See also: :c:type:`display_struct`.
 * :See also: :c:func:`eglib_GetHalFourWireSpiConfigComm`.
 */
typedef struct {
	/**
	 * SPI Mode:
	 *  - 0: CPOL=0, CPHA=0
	 *  - 1: CPOL=0, CPHA=1
	 *  - 2: CPOL=1, CPHA=0
	 *  - 3: CPOL=1, CPHA=1
	 */
	uint8_t mode;

	/** Endianness */
	enum hal_bit_numbering_t bit_numbering;

	/** Delay after CS asserted */
	uint32_t cs_setup_ns;
	/** Delay before de-asserting CS */
	uint32_t cs_hold_ns;
	/** Delay after de-asserting CS */
	uint32_t cs_disable_ns;

	/** Delay before changing DC */
	uint32_t dc_setup_ns;
	/** Delay after changing DC */
	uint32_t dc_hold_ns;

	/** SCK Period (1/frequency*10^9) */
	uint32_t sck_cycle_ns;
} hal_four_wire_spi_config_t;

/**
 * I2C
 * ---
 */

/**
 * I2C Configuration.
 *
 * :See also: :c:type:`display_struct`.
 * :See also: :c:func:`eglib_GetHalI2cConfigComm`.
 */
typedef struct {
	/** I2C Speed */
	enum hal_i2c_speed_t speed;
	/**
	 * Pointer to a function that returns the 7-bit slave address of the display.
	 *
	 * Some displays have fixed addresses, others have addresses configurable
	 * depending on the state of its I/O pins and some have different addresses
	 * whether command or data is being sent.
	 *
	 * This function delegates this definition to each display driver and is
	 * called by HAL drivers :c:type:`hal_struct` via
	 * :c:func:`eglib_GetI2c7bitSlaveAddr` when needed.
	 *
	 * :param eglib: :c:type:`eglib_t` handle.
	 * :param dc: Whether address command or data (:c:type:`hal_dc_t`).
	 * :return: 7-bit slave address.
	 *
	 * :See also: :c:type:`hal_dc_t`.
	 */
	uint8_t (*get_7bit_slave_addr)(eglib_t *eglib, enum hal_dc_t dc);
	/**
	 * Pointer to a function that sends I2C bytes.
	 *
	 * Each display driver has its own particular way of encoding bytes sent
	 * via I2C, depending on data or command being sent, number of bytes etc.
	 *
	 * This function delegates this encoding to display drivers and is called by
	 * HAL drivers :c:type:`hal_struct` via :c:func:`eglib_I2cSend` when needed.
	*
	 * Some examples of what may be required by a display:
	 *
	 *  - First send a byte informing whether the following bytes are data or
	 *    command.
	 *  - Issue an I2C restart at the end of transmission (by calling
	 *    :c:func:`eglib_CommBegin`).
	 *
	 * :param eglib: :c:type:`eglib_t` handle.
	 * :param i2c_write: Pointer to a function that sends the passed byte via
	 *   I2C.
	 * :param dc: Whether bytes are command or data (:c:type:`hal_dc_t`).
	 * :param bytes: Pointer to an array of bytes to send.
	 * :param length: Number of bytes to send.
	 */
	void (*send)(
		eglib_t *eglib,
		void (*i2c_write)(eglib_t *eglib, uint8_t byte),
		enum hal_dc_t dc,
		uint8_t *bytes,
		uint32_t length
	);
} hal_i2c_config_t;

/**
 * Parallel 8-bit 8080
 * -------------------
 */

/**
 * Parallel 8-bit 8080 configuration.
 *
 * :See also: :c:type:`display_struct`.
 * :See also: :c:func:`eglib_GetHalParallel8bit8080ConfigComm`.
 */
typedef struct {
	/** Delay after CSX asserted. */
	uint32_t csx_setup_ns;
	/** Delay before de-asserting CSX. */
	uint32_t csx_hold_ns;
	/** Delay after de-asserting CSX. */
	uint32_t csx_disable_ns;

	/** Delay after D/CX set. */
	uint32_t dcx_setup_ns;

	/** Write cycle  duration  */
	uint32_t wrx_cycle_ns;
	/** Write cycle high duration. */
	uint32_t wrx_high_ns;
	/** Write cycle low duration. */
	uint32_t wrx_low_ns;
} hal_parallel_8_bit_8080_config_t;

/**
 * HAL Driver
 * ==========
 */

/**
 * HAL driver definition.
 *
 * Aliased as :c:type:`hal_t`.
 */
struct hal_struct {
	/**
	 * Pointer to a function that initializes the peripheral based on:
	 *
	 *  - The HAL driver configuration (:c:func:`eglib_GetHalConfig`).
	 *  - The Display driver bus configuration:
	 *
	 *   - **4-Wire SPI**: (:c:func:`eglib_GetHalFourWireSpiConfigComm`).
	 *   - **I2C**: (:c:func:`eglib_GetHalI2cConfigComm`).
	 *   - **Parallel 8-bit 8080**: (:c:func:`eglib_GetHalParallel8bit8080ConfigComm`).
	 *
	 */
	void (*init)(eglib_t *eglib);
	/**
	 * Pointer to a function that puts the peripheral in sleep mode.
	 *
	 * :See also: :c:func:`eglib_SleepIn`.
	 */
	void (*sleep_in)(eglib_t *eglib);
	/**
	 * Pointer to a function that puts the peripheral out of sleep mode
	 *
	 * :See also: :c:func:`eglib_SleepOut`.
	 */
	void (*sleep_out)(eglib_t *eglib);
	/**
	 * Pointer to a function that delays for given amount of nanoseconds.
	 *
	 * :See also: :c:func:`eglib_DelayNs`.
	 */
	void (*delay_ns)(eglib_t *eglib, uint32_t ns);
	/**
	 * Pointer to a function that sets the reset pin to given ``state``.
	 *
	 * :See also: :c:func:`eglib_SetReset`.
	 */
	void (*set_reset)(eglib_t *eglib, bool state);
	/**
	 * Pointer to a function that gets the busy line state.
	 *
	 * :See also: :c:func:`eglib_GetBusy`.
	 */
	bool (*get_busy)(eglib_t *eglib);
	/**
	 * Pointer to a function that begins communication. The meaning of this
	 * function varies depending on the bus:
	 *
	 *  - **4-Wire SPI**: asserts CS.
	 *  - **I2C**: sends start condition.
	 *
	 *   - When called more than one time before ending communication
	 *     (:c:func:`eglib_CommEnd`) sends restart condition.
	 *
	 * :See also: :c:func:`eglib_CommBegin`.
	 */
	void (*comm_begin)(eglib_t *eglib);
	/**
	 * Sends data. Depending on the bus, additional things must happen prior to
	 * sending the data:
	 *
	 *  - **4-Wire SPI**: Set data or command line.
	 *  - **I2C**: if :c:func:`eglib_ShouldSendI2cSlaveAddr` returns true then the
	 *    slave address returned by :c:func:`eglib_GetI2c7bitSlaveAddr` must be sent.
	 *    Additionally, :c:func:`eglib_I2cSend` must be used to send I2C data,
	 *    so that the required encoding by each display is executed.
	 *
	 * :param eglib: :c:type:`eglib_t` handle.
	 * :param dc: Whether bytes are command or data (:c:type:`hal_dc_t`).
	 * :param bytes: Pointer to an array of bytes to send.
	 * :param length: Number of bytes to send.
	 *
	 * :See also: :c:func:`eglib_Send`.
	 */
	void (*send)(eglib_t *eglib, uint32_t dc, uint8_t *bytes, uint32_t length);
	/**
	 * Ends communication. The meaning of this function varies depending on the
	 * bus:
	 *
	 *  - **4-Wire SPI**: de-asserts CS.
	 *  - **I2C**: sends stop condition.
	 *
	 * :See also: :c:func:`eglib_CommEnd`.
	 */
	void (*comm_end)(eglib_t *eglib);
};

/**
 * HAL Driver Helper Functions
 * ---------------------------
 *
 * The following functions can be used from HAL driver :c:type:`hal_struct`
 * functions.
 */

/**
 * Returns a pointer to the HAL driver configuration that was passed to
 * :c:func:`eglib_Init`.
 */
#define eglib_GetHalConfig(eglib) ((eglib)->hal.config_ptr)


/**
 * Whether in between calls to :c:func:`eglib_CommBegin` and :c:func:`eglib_CommEnd`.
 */
#define eglib_IsCommActive(eglib) ((eglib)->hal.comm_active)

/**
 * Whether I2C slave address should be sent.
 */
#define eglib_ShouldSendI2cSlaveAddr(eglib) ((eglib)->hal.i2c_send_slave_addr)

/**
 * .. _Display driver HAL interface functions:
 *
 * Display driver HAL interface functions
 * ======================================
 *
 * The following functions can be used by
 * :doc:`display drivers <../display/index>` :c:type:`display_struct` to
 * interface with HAL.
 */

/**
 * Delay
 * -----
 */

/** Delay for given number of nanoseconds */
#define eglib_DelayNs(eglib, ns) ((eglib)->hal.driver->delay_ns(eglib, ns))

/** Delay for given number of microseconds */
#define eglib_DelayUs(eglib, us) ((eglib)->hal.driver->delay_ns(eglib, (uint32_t)(us) * 1000UL))

/** Delay for given number of milliseconds */
#define eglib_DelayMs(eglib, ms) ((eglib)->hal.driver->delay_ns(eglib, (uint32_t)(ms) * 1000000UL))

/** Delay for given number of seconds */
void eglib_DelayS(eglib_t *eglib, uint32_t s);

/**
 * I/O
 * ---
 */

/**
 * Set reset line to given state.
 */
#define eglib_SetReset(eglib, state) ((eglib)->hal.driver->set_reset(eglib, state))

/**
 * Get the value of th busy data line, often found in e-ink / e-paper displays.
 */
#define eglib_GetBusy(eglib) ((eglib)->hal.driver->get_busy(eglib))

/**
 * Communication
 * -------------
 */

/** Initiates communication. Must be called before :c:func:`eglib_Send`. */
void eglib_CommBegin(eglib_t *eglib);

/**
 * Sends data. Must be called after :c:func:`eglib_CommBegin`.
 *
 * :param eglib: :c:type:`eglib_t` handle.
 * :param dc: Whether bytes are command or data (:c:type:`hal_dc_t`).
 * :param bytes: Pointer to an array of bytes to send.
 * :param length: Number of bytes to send.
 */
void eglib_Send(eglib_t *eglib, enum hal_dc_t dc, uint8_t *bytes, uint32_t length);

/**
 * Sends data.
 *
 * :See also: :c:func:`eglib_Send`.
 */
#define eglib_SendData(eglib, bytes, length) (\
	eglib_Send(eglib, HAL_DATA, bytes, length)\
)

/**
 * Sends a single data byte.
 *
 * :See also: :c:func:`eglib_Send`.
 */
#define eglib_SendDataByte(eglib, bytes) (\
	eglib_SendData(eglib, &((uint8_t){bytes}), 1)\
)

/**
 * Sends commands.
 *
 * :See also: :c:func:`eglib_Send`.
 */
#define eglib_SendCommands(eglib, bytes, length) (\
	eglib_Send(eglib, HAL_COMMAND, bytes, length)\
)

/**
 * Sends a single command byte.
 *
 * :See also: :c:func:`eglib_Send`.
 */
#define eglib_SendCommandByte(eglib, bytes) (\
	eglib_SendCommands(eglib, &((uint8_t){bytes}), 1)\
)

/** Ends communication. */
void eglib_CommEnd(eglib_t *eglib);

#endif

#include "display.h"
