HAL
===

The Hardware Abstraction layer provides a common interface :c:type:`hal_struct`, implemented by HAL drivers, that enable :doc:`display drivers<../display/index>` to talk to hardware, regardless of the data bus or platform used.

.. toctree::
   :maxdepth: 3
   :caption: Contents:

   interface.rst
   four_wire_spi/index.rst
   i2c/index.rst
   parallel_8_bit_8080/index.rst