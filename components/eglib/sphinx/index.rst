eglib
=====

A portable C library for interfacing with LCD, OLED, e-ink / e-paper displays, usable from microcontrollers to Linux.

Its design provides a simple but coherent interface enjoyable from both the perspective of end users or HAL / display driver developers.

The name *eglib* originally came from *Experimental Graphics Library*, but as it matured, *e* can stand for *exceptional*, *extraordinary*, *exquisite*, *elegant*...

Features
--------

- Hardware Abstraction Layer (HAL).

  - Supports different data buses such as 4-Wire SPI and I2C.
  - Easy to develop reusable drivers for different platforms.
    - Drivers have accept configuration for things like I/O pins.
    - Display drivers provide bus specific configuration.

- Display drivers.

  - Common interface regardless of data bus.
  - Supports configuration

    - Same controller driver works with different screen sizes or boards.
    - Allows configuration of things like color depth.

  - Support for specific commands: color inversion, scrolling, brightness etc.
  - Accelerated line drawing routines.
  - Supports a wide range of:

    - Display types: LCD, OLED, e-ink/e-paper.
    - Color depths: black/white, black/white/red|yellow (e-ink / e-paper), grayscale to full color.

- Frame buffer

	- Can be used on top of any display driver.
	- Faster refresh rates.
	- No visual artifacts from drawing directly on the display memory.
	- Partial display updates.

- Drawing

  - Many functions for lines, boxes, circle etc.
  - Clipping box support.

.. toctree::
   :maxdepth: 4
   :caption: Contents:
   :hidden:

   driver_development/index.rst
   api_reference/index.rst