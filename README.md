# XCVario
Software for ESP32 based lean Variometer system with open data interface featuring OpenVario, Cambridge, Borgelt and XCVario format, running on dedicated XCVario hardware

![Vario]( https://raw.githubusercontent.com/iltis42/XCVario/master/images/Vario3D/vario-perspectiv.png )
![Vario Back]( https://github.com/iltis42/XCVario/blob/master/images/Vario3D/backviev-xcv-21.png )

The project is dedicated to my open source and open hardware flight computer development using ESP32 WROOM development board supporting modern digital sensors such as BMP280, SPL06-007, DS1820b, TE4525, ABPMRR, MP5004DP, CAN, AHRS MPU6050, and QMC5883L magnet sensor.

* Evolution
The history of the project is is available within branches and is listed below. 

1. [DM-R1](https://github.com/iltis42/XCVario/tree/DM-R1) branch
The inital 'Dot Matrix' release 1 features a 128x64 pixel monocrome LCD display. It delivers already all data in the well known and documented OperVario format to any flight computer running XC-Soar, but had limitation's in grafical resolution. Beside vario, speed to fly command arrow and altitude, further informations like battery voltage, temperature and MC value were mapped to a single field that changes every 3 seconds. A handful of prototypes has been build and tested in air with several flight computers. The sensor board with 47x102 mm hosts several modules and is mounted horizontally in a cyclindric housing through the I-Panel, while the display keeps in front. Also the first revision had a responsive kalman filter, that delivers a smooth response without the typical delay of several seconds in simple low pass filters. The vario is optimized to display exactly what you feel when entering lift.

2. [LV-R2](https://github.com/iltis42/XCVario/tree/LV-R2) branch
The 'Long Vario' release 2 features a new technology 320x240 pixel IPS LCS display, readable from almost any angle (80/80/80) and with 500 nits brightness it performs well in sunlight. The sensor board and mostly the housing are the same as in the DM-R1 type, but as the display has higher resolution and features color, the arrangement of data on the display has been reworked, and more things were added like a moving gauge for airspeed (IAS), a permanant display of MC value, temperature and battery charge state and bluetooth connection indication. The color is used to visualize positive and netative values. A low number of prototypes has been made by use of a 3D printed ABS housing, shielded by copper foil and mounted from front of I-Panel. 

3. [R3-20](https://github.com/iltis42/XCVario/tree/R3-20) branch
The third generation 'XC Vario' uses the same display as the long vario, but came with a new sensor board 61x64 mm, vertically mounted behind the display board. The sensor board has a RJ45 connector for electrical connections, digital potentiomenter, and some more features like software update over the air, serial output and serial to bluetooth bridge and connector for external audio. It still has an audio amplifier with 1.2 Watt and an efficient loudspeaker on board, and has a just 34 mm long CNC fabricated aluminium housing and can be from back of the I-Panel as any other standard 57 mm instrument. A three digit number of boards have been fabricated and has been offered via .de webshop, selling the hardware for an interresting price.

4. master branch (default, what you see here)
The fourth generation XCVario or XCVario series 21,22,23 has an enhanced feature set and has been pimped up with some nice hardware gimicks. The device's got a second serial interface (S2) following the IGC standard pinout to be open for more applications with external serial devices. The interface pins RX/TX are programmable to be able to swap RX/TX lines by setup to minimize need for custom cables, so mostly a standard 1:1 patch  cable can be used. The second interface S2 can also power external IGC compatible devices and the same connector features one addittional programmable analog or digital input, e.g. for flarm sensor application.  Furthermore a 6 axis 'Attitude and Heading Reference System' (AHRS) has been placed on the board to provide acceleration to the flight computing software plus pitch and angle of bank for a connected XCSoar application. As extenal hardware a flap sensor has been developed, indication your current flap position on the display, plus a high precision magnetic sensor board featuring the new QMC5883L chip can both be connected to S2 interface. The shop opened January 2021 with a three digit number of new 2021 hardware devices http://xcvario.com/store webshop. Series 22 devices were extended by a CAN interface. The digital audio amplifier has an increased power of 3 Watt's in series 23 providing less THD at high volumes, an antiglare and anti-fingerprint display was added, and the IMU got a temperature control for lower temperature related drift. 

[List of Software Releases](https://github.com/iltis42/XCVario/releases/) , latest release on the top <br>

[Ongoing Development](https://github.com/iltis42/XCVario/tree/master/images) ,latest beta software is the one with the highest revision number or newest date

At the begin i want to show a short Demo of the variometer, including sensitivity and full sunlight test, followed by an explanation of various setup options and how to use. Its now year four of this project, demo videos with prototypes based on the hardware from LV-R2 branch see here below:


* [Quick Demo on YouTube](https://www.youtube.com/watch?v=Piu5SiNPaRg)
* [Vario Sensitivity Test](https://www.youtube.com/watch?v=RqFLOQ9wvgY)
* [Basic Function Demo](https://www.youtube.com/watch?v=zGldyS57ZgQ)
* [Full Sunlight test](https://www.youtube.com/watch?v=TFL9i2DBNpA)
* [Audio Demo in Flight](https://www.youtube.com/watch?v=6Vc6OHcO_T4)  ("Single Tone" Modus)
* [Basic Vario Setup MC, QNH, Ballast](https://www.youtube.com/watch?v=DvqhuaVlfEI)
* [Airfield Elevation and Vario Setup](https://www.youtube.com/watch?v=x3UIpL9qGec)
* [Bluetooth, Audio and Polar Setup](https://www.youtube.com/watch?v=9HcsfyLX-wE)
* [Flap (WK) Indicator Setup](https://www.youtube.com/watch?v=tP2a2aDoOsg)
* [Vario System Setup](https://www.youtube.com/watch?v=BCR16WUTwJY)
* [Setup XCSoar to connect with Vario](https://www.youtube.com/watch?v=LDgnvLoTekU&t=95s)



The design supports the follwing sensors:
* TE Variometer (TE)
* Airspeed (PI)
* Barometric Altitude (BA)
* Outside Temperature
* Battery Voltage
* Acceleration and Rotation
* Optional Earth magnetic field

The ESP32 module contains a bluetooth and WLAN sender module plus serial interfaces, so we are able to transmit wireless or wired all data to any navigation device running XCSoar, LK8000 and more in various formats as there is the native XCVario, Borgelt, Cambridge and Openvario format,so devices can operate as full glide computer with TE-Vario, Barometric Altitude, Speed and more.

* [Handbook pdfs ge/en per release](https://github.com/iltis42/XCVario/blob/master/handbook)  -> obsolete (for older releases)
* [Latest Handbook Germann, English, even Multilingual](https://xcvario.com/docs-category/vario)

The Vario Prototype with 550 nits 2.4 inch IPS Technology LCD Display features a low power consumation of less that 1.2W, that equals to 100mA at 12.5V operating voltage. The Size is 68(H)x64(B)x35(T) mm, for a standard 57mm Instrument Panel. Weight is only about 170 gramm.

The Soft- and Hardware features:

- QNH Ballast, Bugs and McCready adjustment and many other options configurable
- Audio-Generator with adjustable Volume and Deadband plus setup option for frequency, tone style and range
- Integrated Loudspeaker, 2 Watt Audio power
- Optimum Flap Position Indicator with Flap Position Sensor (optional)
- S2F (Speed2Fly) Indicator with configurable MC Ballast and Bugs based on customizable Polar
- Around 130 Polars are included in installed polar library ( list in https://github.com/iltis42/XCVario/blob/master/main/Polars.cpp )
- IAS or TAS airspeed indication
- OAT (Outside Air Temperature) Sensor
- Battery Charge Indicator, adaptable to battery type
- Variometer display with adjustable range (1 m/s - 30 m/s), and damping ( 1s - 10s )
- Bluetooth, Wifi and two serial interfaces to external devices (XCSoar, LK8000, FLARM, Sensors, etc)
- Biderectional Sync of QNH, MacCready Value, Ballast and Bugs with secondary variometer and navi
- High precision barometric Altimeter 1 hPa (8 meter) absolute accuracy, and 0.1 meter resolution
- Sunlight readable high contrast 550 nits 2.4 inch IPS Technology Display
- Battery Voltage Indicator
- Switch for S2F/Vario Mode (Audio and Netto-Mode)
- Detailed Setup Menu to customise Vario for various applications (Standalone, Twin-Seater, etc.)
- Option to connect navi to secondary device
- Serial to wirless (Bluetooth or WiFi Bridge) e.g. for FLARM and FLARM alert screen
- Output of NMEA data in OpenVario,Borgelt,Cambridge or XCVario format
- OTA (Over The Air) Software Update via WiFi and Browser
- Attitude and Heading Reference System (AHRS) available since 2021 hardware revision
- Wind calculation in circling (with a Flarm) and in straight flight by magnet sensor extension

