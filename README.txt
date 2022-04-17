Building the XCVario Software:

XCVario is build using esp-idf (https://github.com/espressif/esp-idf), to be setup as described here:
https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/#get-started-get-esp-idf
As OS to crossbuild XCVario software, linux e.g. latest ubuntu is strongly recommended.

Another option is to fork XCVario in your own area, and use a github runner to build the project remotely on github.
As an example check this repository Actions workflows, where binary is build by the following this yml:
https://github.com/iltis42/XCVario/blob/master/.github/workflows/cmake.yml

Here the instructions to build the binary locally:

1)  Clone, checkout branch: release/v4.3 install and activate esp-idf and get cmake (obviously cmake is missing from install.sh)
mkdir -p ~/esp; cd ~/esp; git clone --recursive https://github.com/espressif/esp-idf.git;
cd ~/esp/esp-idf; 
git checkout release/v4.3; 
./install.sh; . ./export.sh;
pip install cmake;

This will get the stable branch release/v4.3 of esp-idf plus corresponding compiler, etc.
For the docker official releases, branch label is release-v4.3, see also workflow yaml.

2) Build hello world application.
A good idea is to work through the get-started manual from esp-idf, select the versions as shown,
patch the esp-idf as shown below in the Appendix, and build the hello world example from esp-idf, load it and run the monitor on the XCVario:

The software is build using cmake, and flashed and monitored by calling e.g. "idf.py build" 
from the directory of the software to be build. The workflow for each is shown here below:

cd ~/esp/esp-idf/examples/get-started/hello_world;
idf.py build
WARNING: Support for Python 2 is deprecated and will be removed in future versions.
Executing action: all (aliases: build)
Running cmake in directory /home/gittest2/esp/esp-idf/examples/get-started/hello_world/build
Executing "cmake -G 'Unix Makefiles' -DPYTHON_DEPS_CHECKED=1 -DESP_PLATFORM=1 -DCCACHE_ENABLE=0 /home/gittest2/esp/esp-idf/examples/get-started/hello_world"...
-- Found Git: /usr/bin/git (found version "2.17.1") 
-- IDF_TARGET not set, using default target: esp32
-- The C compiler identification is GNU 8.4.0
:
[100%] Built target app
Project build complete. To flash, run this command:
/home/gittest2/.espressif/python_env/idf4.3_py2.7_env/bin/python ../../../components/esptool_py/esptool/esptool.py -p (PORT) -b 460800 --before default_reset --after hard_reset --chip esp32  write_flash --flash_mode dio --flash_size detect --flash_freq 40m 0x1000 build/bootloader/bootloader.bin 0x8000 build/partition_table/partition-table.bin 0x10000 build/hello-world.bin
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
WARNING: Support for Python 2 is deprecated and will be removed in future versions.
Executing action: monitor
Running idf_monitor in directory /home/gittest2/esp/esp-idf/examples/get-started/hello_world
Executing "/home/gittest2/.espressif/python_env/idf4.3_py2.7_env/bin/python /home/gittest2/esp/esp-idf/tools/idf_monitor.py -p /dev/ttyUSB0 -b 115200 --toolchain-prefix xtensa-esp32-elf- /home/gittest2/esp/esp-idf/examples/get-started/hello_world/build/hello-world.elf -m '/home/gittest2/.espressif/python_env/idf4.3_py2.7_env/bin/python' '/home/gittest2/esp/esp-idf/tools/idf.py' '-p' '/dev/ttyUSB0'"...
--- idf_monitor on /dev/ttyUSB0 115200 ---
--- Quit: Ctrl+] | Menu: Ctrl+T | Help: Ctrl+T followed by Ctrl+H ---
I (30) boot: ESP-IDF v4.3-dev-2586-g526f68239 2nd stage bootloader
I (30) boot: compile time 11:40:17
I (30) boot: chip revision: 1
I (34) boot_comm: chip revision: 1, min. bootloader chip revision: 0
I (42) boot.esp32: SPI Speed      : 40MHz
I (46) boot.esp32: SPI Mode       : DIO
I (51) boot.esp32: SPI Flash Size : 2MB

After this is done successfully, then clone the XCVario into the examples directory, parallel to the example directory hello_world, and do the same from there, and voila the control is yours:

A) Clone XCVario repository:
cd ~/esp/esp-idf/examples/get-started/; git clone https://github.com/iltis42/XCVario.git

B) Build software and flash XCVario Software, e.g. shown as here via USB cable, or use OTA
cd XCVario
idf.py build

# If you change files in the build environment, issue the command:

idf.py reconfigure

# In order to flash the binary through micro USB connector inside the device:
idf.py -p /dev/ttyUSB0 flash
# Then you may monitor loggings and printouts from USB serial console here:
idf.py -p /dev/ttyUSB0 monitor

C) Flashing via OTA
Start OTA Software download Wifi AP at XCVario and
upload through this webpage binary image: ~/esp/esp-idf/examples/get-started/XCVario/build/sensor.bin

