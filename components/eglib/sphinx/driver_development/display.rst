Display
=======

.. contents::
    :depth: 3

Overview
--------

The general process of writing a display driver is as follows:

1. Find a display controller IC datasheet.

  A quick web search for its number usually finds a PDF datasheet.

  Displays are often sold as "modules" where the display, controller IC and a printed circuit board (that breaks out I/O pins) are all sandwiched together. Vendors of such boards usually inform the IC number.

2. Determine which data buses it can talk to.

  It is common for displays controllers to support multiple buses: 4-Wire SPI, 3-Wire SPI, I2C, Parallel 6800/8000.

  eglib has support for some of those with its :doc:`../api_reference/hal/index` interface.

3. Determine all commands & arguments it can accept.

  Datasheets usually have a "commands" section. Those must be sorted out in different buckets:

  - **Configuration**: Some commands accept arguments that must be exposed by the display driver configuration. Examples of such commands are setting the number of rows / columns, color depth, voltage regulators etc. Having these commands configurable enables the driver to be easily reused for different display sizes or display module boards.
  - **Extra functions**: Some displays support specific functions such as brightness, scrolling and inversion. Those can be exposed as display specific functions.
  - **Driver internal**: Those commands do things like initialization, sleep in/out and need not to be exposed outside of the driver.
  - **Read**: eglib only supports sending data to displays, these can be ignored.

4. Determine the display initialization sequence.

  Datasheets often have sections explaining their initialization command sequence. Unfortunately, datasheets are not always clear on this and this initialization is usually a function of how the display driver is physically connected to the display and its circuit board. It is common for vendors to share example code for interfacing with their displays (eg: for Arduino, STM32 etc). Those can be a great source for finding the initialization sequence.

5. Determine how to write pixels to the display memory.

  Usually there are commands to set the row / column which can be followed by pixel data for that address. Some displays can auto-increment/decrement the memory address, meaning the next data write will be for a pixel adjacent to the previous. This can be leveraged to accelerate line drawing routines.

6. Write the driver.

7. Write unit tests.

8. Send a pull request.

Scaffold the Code
-----------------

Fork `eglib's Git <https://github.com/fornellas/eglib/>`_ and clone it:

.. code:: bash

  git clone https://github.com/$YOUR_GH_USERNAME/eglib/
  cd eglib/

Then add ``src/eglib/display/${IC_NUMBER}.h``:

.. code:: c

  #include "../display.h"
  
  typedef struct {
  	// Add entries here for all configuration that must be sent as commands
  } ic_number_config_t;
  
  extern const display_t ic_number;
  
  void ic_number_DoSomethingSpecific(eglib_t *eglib);

Then add ``src/eglib/display/${IC_NUMBER}.c``. This must define the :c:type:`display_t` ``ic_number`` variable declared at the header. This variable type is actually :c:type:`display_struct`, which must be filled with:

- HAL communication configuration for each supported bus.
- Pointers to functions that implement the various bits of the driver.

:c:type:`display_struct` is well documented: you should be able to follow it though to a working driver.

:Important: Create defines for all display commands used: sending a "maigic" value ``0x33`` is meaningless, but ``IC_NUMBER_SLEEP_IN`` is not.

:Tip: Peek at other drivers implementations at `src/eglib/display/ <https://github.com/fornellas/eglib/tree/master/src/eglib/display>`_ for reference.

Add An Example
--------------

Add a working example at `examples/ <https://github.com/fornellas/eglib/tree/master/examples/>`_, including a ``Makefile`` for it.

:Important: test your example with real hardware to make sure it works.

Add Documentation
-----------------

Peek at other existing drivers at `src/eglib/display/ <https://github.com/fornellas/eglib/tree/master/src/eglib/display>`_ to see how the documentation inside the C code should be.

Add a documentation page ``sphinx/reference/display/drivers/${IC_NUMBER}.rst``, following the `example of other display drivers <https://github.com/fornellas/eglib/tree/master/sphinx/reference/display/drivers>`_. Also add this to `sphinx/reference/display/index.rst <https://github.com/fornellas/eglib/blob/master/sphinx/reference/display/index.rst>`_

Add Unit Tests
--------------

Add one or more tests to `master/tests/display <https://github.com/fornellas/eglib/tree/master/tests/display>`_.

Send A Pull Request
-------------------

Send a `pull request <https://docs.github.com/en/free-pro-team@latest/github/collaborating-with-issues-and-pull-requests/creating-a-pull-request>`_ of your good work, so others can benefit from it.

Questions?
----------

Please `reach out <https://github.com/fornellas/eglib/issues>`_.