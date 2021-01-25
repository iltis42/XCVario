Building the XCVario Software:

XCVario is build using esp-idf (https://github.com/espressif/esp-idf), to be setup as described here:
https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/#get-started-get-esp-idf
As OS to crossbuild XCVario software, linux e.g. latest ubuntu is strongly recommended.

1)  Clone, install, activate esp-idf and get cmake (obviously cmake is missing from install.sh)
mkdir -p ~/esp; cd ~/esp; git clone --recursive https://github.com/espressif/esp-idf.git;
cd ~/esp/esp-idf; ./install.sh; . ./export.sh;
pip install cmake;


This will get you the lates state of esp-idf plus compiler (at this time is: esp-2020r3-8.4.0)

2) Build hello world application.
A good idea is to work through the get-started manual from esp-idf, select the versions as shown,
patch the esp-idf as shown below in the Appendix, and build the hello world example from esp-idf, load it and run the monitor on the XCVario:

The software is build using cmake, and flashed and monitored by calling e.g. "idf.py build" 
from the directory of the software to be build. The workflow for each is shown here below:

cd ~/esp/esp-idf/examples/get-started/hello_world;
idf.py build
Executing action: all (aliases: build)
Running make in directory /home/nextpcb/esp/esp-idf/examples/get-started/hello_world/build
Executing "make -j 6 all"...
:
[100%] Generating binary image from built executable
esptool.py v3.0-dev
Generated /home/nextpcb/esp/esp-idf/examples/get-started/hello_world/build/hello-world.bin
[100%] Built target gen_project_binary
[100%] Built target app

Project build complete. To flash, run this command:
/home/xcvario/.espressif/python_env/idf4.2_py2.7_env/bin/python ../../../components/esptool_py/esptool/esptool.py -p (PORT) -b 460800 --before default_reset --after hard_reset --chip esp32  write_flash --flash_mode dio --flash_size detect --flash_freq 40m 0x1000 build/bootloader/bootloader.bin 0x8000 build/partition_table/partition-table.bin 0x10000 build/hello-world.bin
or run 'idf.py -p (PORT) flash'

$ idf.py -p /dev/ttyUSB0 flash
Executing action: flash
:
Writing at 0x00020000... (100 %)
Wrote 150112 bytes (79178 compressed) at 0x00010000 in 1.8 seconds (effective 654.5 kbit/s)...
Hash of data verified.

Leaving...
Hard resetting via RTS pin...
[100%] Built target flash
Done

$ idf.py -p /dev/ttyUSB0 monitor
Executing action: monitor
:
I (28) boot: ESP-IDF v4.2-dev-1905-g625bd5eb1-dirty 2nd stage bootloader
I (28) boot: compile time 10:16:30
I (30) boot: chip revision: 1
I (33) boot_comm: chip revision: 1, min. bootloader chip revision: 0
I (40) boot.esp32: SPI Speed      : 40MHz
:
I (312) cpu_start: Starting scheduler on PRO CPU.
I (0) cpu_start: Starting scheduler on APP CPU.
Hello world!
This is esp32 chip with 2 CPU cores, WiFi/BT/BLE, silicon revision 1, 2MB external flash
Free heap: 299596
Restarting in 10 seconds...

After this is done successfully, then clone the 
XCVario into the examples directory, parallel to the example directory hello_world,
and do the same from there, and voila the control is yours:

A) Clone XCVario repository:
cd ~/esp/esp-idf/examples/get-started/; git clone https://github.com/iltis42/XCVario.git;

B) Build software and flash
cd XCVario; 
idf.py build;
idf.py -p /dev/ttyUSB0 flash;
idf.py monitor;

Appendix: 

The esp-idf needs to be patched:

1) Open I2C bug: https://github.com/espressif/esp-idf/issues/5108
The native i2c driver has a multithreading bug from using malloc and free, that will lead to a crash
as of multiple chips/tasks are using malloc and free what makes the system to crash regularly.
The issue is still open after a year, even provided a solution it has beed discussed in another
direction, hence also this solution didn't come. So for now only my workaround is possible: 


