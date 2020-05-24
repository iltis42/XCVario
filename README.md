# OpenIVario
ESP32 based lean Variometer system with OpenVario Interface

At the begin i want to show a short Demo of the variometer, including sensitivity and full sunlight test, followed by an explanation of various setup options and how to use. Its now the third year of this project, moving now to a colored display this winter we had built a low number of devices with a 3D printed housing. Demo vidoes with prototypes based on that see below. The second generation vario with some more features uses a smaller CNC fabricated aluminium housing and there is a bigger lot of devices now planned.

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


The project is dedicated to my new flight computer development using ESP32 WROOM development board plus modern sensors such as BMP280, DS1820b, MP5004DP, and colored sunlight readable display display.
It delivers data in the well known and documented OperVario format to any flight computer running XC-Soar.

The design supports the follwing sensors:
* TE Variometer
* Airspeed
* Barometric Altitude
* Outside Temperature
* Battery Voltage

The ESP32 module contains a bluetooth sender module, so we are able to transmit all data to XCSoar in [OpenVario format](https://github.com/iltis42/OpenIVario/blob/master/putty.log), so XCSoar can operate as full glide computer with TE-Vario, Barometric Altitude, Speed and more.

The Vario Prototype with 500 nits 2.4 inch IPS Technology LCD Display features a low power consumation of less that 1W, that equals to 70mA at 12.5V operating voltage. The Size is 68(H)x64(B)x35(T) mm, for a standard 57mm Instrument Panel. Weight is below 300 gramm.

The Soft- and Hardware features:

- Easy QNH and McCready Adjustment
- Audio-Generator with adjustable Volume and Deadband plus setup option for frequency tone style and range
- Integrated Loudspeaker 
- Flap Position Indicator Option
- S2F (Speed2Fly) Indicator with configurable MC and Ballast based on customizable Polar
- More that 80 Polars are already included ( list in https://github.com/iltis42/OpenIVario/blob/master/main/Polars.cpp )
- IAS (Indicated Airspeed) Indicator
- Temperatur Indicator
- Battery Charge Indicator
- Variometer display with adjustable range (1 m/s - 30 m/s), and damping ( 1s - 10s )
- Bluetooth Interface for Android devices running XC Soar (OpenVario)
- High precision barometric Altimeter 1 hPa (8 meter) absolute accuracy
- Sunlight readable 500 nits 2.4 inch IPS Technology Display
- Battery Voltage Indicator
- Audio Switch for Vario/S2F Audio
- Detailed Setup Menu
- Serial to Bluetooth Bridge e.g. for FLARM
- Serial Output of OpenVarion Format NMEA data



![IPS Display 2.4 inch]( https://raw.githubusercontent.com/iltis42/OpenIVario/master/images/Vario3D/vario-perspectiv.png )

