/**
  *
  * UBX_Parser.h - A header-only C++ class for parsing UBX messages from Ublox GPS
  *
  * Copyright (C) 2015 Simon D. Levy
  *
  * This code is free software: you can redistribute it and/or modify
  * it under the terms of the GNU Lesser General Public License as 
  * published by the Free Software Foundation, either version 3 of the 
  * License, or (at your option) any later version.
  * 
  * This code is distributed in the hope that it will be useful,     
  * but WITHOUT ANY WARRANTY without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  * 
  * You should have received a copy of the GNU Lesser General Public License 
  * along with this code.  If not, see <http://www.gnu.org/licenses/>.
  */

/**
  * A class for parsing UBX messages.
  */
#include "UBX_Parser.h"
typedef enum {

            GOT_NONE,
            GOT_SYNC1,
            GOT_SYNC2,
            GOT_CLASS,
            GOT_ID,
            GOT_LENGTH1,
            GOT_LENGTH2,
            GOT_PAYLOAD,
            GOT_CHKA

        } state_t;
state_t state;

int msgclass;
int msgid;
int msglen;
char chka;
char chkb;
int count;
char payload[1000];

void addchk(int b) {

            chka = (chka + b) & 0xFF;
            chkb = (chkb + chka) & 0xFF;
        }


void dispatchMessage() {//La payload est entièrement reçue et le checkSum est correct

            switch (msgid) {
                case 0x02: 
                    {
                    unsigned long iTOW = (unsigned long)unpack_int32(0);
                    long lon = unpack_int32(4);
                    long lat = unpack_int32(8);
                    long height = unpack_int32(12);
                    long hMSL = unpack_int32(16);
                    unsigned long hAcc = (unsigned long)unpack_int32(20);
                    unsigned long vAcc = (unsigned long)unpack_int32(24);
                    handle_NAV_POSLLH(iTOW, lon, lat, height, hMSL, hAcc, vAcc);
                    }
                    break;
                case 0x04: 
                    {
                    unsigned long iTOW = (unsigned long)unpack_int32(0);
                    unsigned short gDOP = (unsigned short)unpack_int16(4);
                    unsigned short pDOP = (unsigned short)unpack_int16(6);
                    unsigned short tDOP = (unsigned short)unpack_int16(8);
                    unsigned short vDOP = (unsigned short)unpack_int16(10);
                    unsigned short hDOP = (unsigned short)unpack_int16(12);
                    unsigned short nDOP = (unsigned short)unpack_int16(14);
                    unsigned short eDOP = (unsigned short)unpack_int16(16);
                    handle_NAV_DOP(iTOW, gDOP, pDOP, tDOP, vDOP, hDOP, nDOP, eDOP);
                    }
                    break;
                case 0x07:
                    {
                    unsigned long iTOW = (unsigned long)unpack_int32(0);
                    unsigned short year = (unsigned short)unpack_int16(4);
                    unsigned char month = payload[6];
                    unsigned char day = payload[7];
                    unsigned char hour = payload[8];
                    unsigned char minute = payload[9];
                    unsigned char seconde = payload[10];
                    unsigned char validate = payload[11];
                    unsigned long tAcc = (unsigned long)unpack_int32(12);
                    long nano = unpack_int32(16);
                    unsigned char fixType = payload[20];
                    unsigned char flags = payload[21];
                    unsigned char flags2 = payload[22];
                    unsigned char numSV = payload[23];
                    long lon = unpack_int32(24);
                    long lat = unpack_int32(28);
                    long height = unpack_int32(32);
                    long hMSL = unpack_int32(36);
                    unsigned long hAcc = (unsigned long)unpack_int32(40);
                    unsigned long vAcc = (unsigned long)unpack_int32(44);
                    long velN = unpack_int32(48);
                    long velE = unpack_int32(52);
                    long velD = unpack_int32(56);
                    long gSpeed = unpack_int32(60);
                    long headMot = unpack_int32(64);
                    unsigned long sAcc = (unsigned long)unpack_int32(68);
                    unsigned long headAcc = (unsigned long)unpack_int32(72);
                    unsigned short pDOP = (unsigned long)unpack_int16(76);
                    unsigned char flags3 = payload[78];
                    unsigned char reserved01 = (unsigned char)unpack_int16(79);
                    unsigned long reserved02 = (unsigned long)unpack_int32(80);
                    long headVeh = unpack_int32(84);
                    short magDec = unpack_int16(88);
                    unsigned short magAcc = (unsigned long)unpack_int16(90);
                    handle_NAV_PVT(iTOW, year, month, day, hour, minute, seconde, validate,tAcc,nano,
                    		fixType, flags, flags2, numSV,lon, lat, height, hMSL, hAcc, vAcc,
							velN, velE, velD, gSpeed, headMot, sAcc, headAcc, pDOP, flags3, reserved01,reserved02,
							headVeh,magDec,magAcc);
                    }
                    break;
                case 0x12: 
                    {
                    unsigned long iTOW = (unsigned long)unpack_int32(0);
                    long velN = unpack_int32(4);
                    long velE = unpack_int32(8);
                    long velD = unpack_int32(12);
                    unsigned long speed = (unsigned long)unpack_int32(16);
                    unsigned long gSpeed = (unsigned long)unpack_int32(20);
                    long heading = unpack_int32(24);
                    unsigned long sAcc = (unsigned long)unpack_int32(28);
                    unsigned long cAcc = (unsigned long)unpack_int32(32);
                    handle_NAV_VELNED(iTOW, velN, velE, velD, speed, gSpeed, heading, sAcc, cAcc);
                    }
                    break;
                    /*
                case 0x06: 
                    {
                    unsigned long iTOW = (unsigned long)unpack_int32(0);
                    long fTOW = unpack_int32(4);
                    short week = (short)unpack_int16(8);
                    char gpsFix = payload[10];
                    handle_NAV_SOL(iTOW, fTOW, week, gpsFix);
                    }
                    break;
                    */
                  default:
                    reportUnhandled(msgid);
                    break;
            }
        }

long unpack_int32(int offset) {

            return unpack(offset, 4);
        }

long unpack_int16(int offset) {

            return unpack(offset, 2);
        }

long unpack(int offset, int size) {

            long value = 0; // four bytes on most Arduinos

            for (int k=0; k<size; ++k) {
                value <<= 8;
                value |= (0xFF & payload[offset+4-k-1]);
            }

            return value;
         }

        /**
          Override this method to handle NAV-POSLLH messages.
          @param iTOW GPS Millisecond Time of Week
          @param lon Longitude in degrees * 10<sup>7</sup>
          @param lat Latitude in degrees * 10<sup>7</sup>
          @param height Height above Ellipsoid in millimeters
          @param hMSL Height above mean sea level in millimeters
          @param hAcc Horizontal Accuracy Estimate in millimeters
          @param vAcc Vertical Accuracy Estimate in millimeters
          */
void handle_NAV_POSLLH(unsigned long iTOW,
                long lon, 
                long lat, 
                long height, 
                long hMSL, 
                unsigned long hAcc, 
                unsigned long vAcc) { }

        /**
          Override this method to handle NAV-DOP messages.
          @param iTOW GPS Millisecond Time of Week
          @param gDOP Geometric DOP
          @param pDOP Posiition DOP
          @param tDOP Time DOP
          @param vDOP Vertical DOP
          @param hDOP Horizontal DOP
          @param nDOP Northing DOP
          @param eDOP Easting DOP
          */
void handle_NAV_DOP(unsigned long iTOW,
                unsigned short gDOP,
                unsigned short pDOP,
                unsigned short tDOP,
                unsigned short vDOP,
                unsigned short hDOP,
                unsigned short nDOP,
                unsigned short eDOP) { }

        /**
          Override this method to handle NAV-VELNED messages.
          @param iTOW GPS Millisecond Time of Week
          @param velN NED north velocity in cm/sec
          @param velE NED east velocity in cm/sec
          @param velD NED down velocity in cm/sec
          @param speed Speed (3-D)in cm/sec
          @param gSpeed Ground Speed (3-D)in cm/sec
          @param heading Heading of motion 2-D in degrees * 10<sup>5</sup>
          @param sAcc Speed Accuracy Estimate in cm/sec
          @param cAcc Course / Heading Accuracy Estimate in degrees
          */
void handle_NAV_VELNED(unsigned long iTOW,
                long velN,
                long velE,
                long velD,
                unsigned long speed,
                unsigned long gSpeed,
                long heading,
                unsigned long sAcc,
                unsigned long cAcc) { }

        /**
          Override this method to handle NAV-SOL messages.
          @param iTOW GPS Millisecond Time of Week
          @param fTOW fractional time of week in nanoseconds
          @param week GPS week number of the navigation epoch
          @param gpsFix GPS fix type
          */
         /*
         virtual void handle_NAV_SOL(unsigned long iTOW, 
                 long fTOW, 
                 short week, 
                 char gpsFix) { }
                 */

         /**
           * Override this method to report receipt of messages not
           * handled by current code.
           * @param msgid ID of current message
           */
void handle_NAV_PVT(unsigned long iTOW, /* ms*/
		        unsigned short year,	/*Year UTC*/
				unsigned char month,	/* Month 1..12*/
				unsigned char day,		/* Day 1..31*/
				unsigned char hour,		/*Hour of the day 1..23 UTC*/
				unsigned char minute,	/* Minutes 0..59*/
				unsigned char seconde,	/* Secondes 0..59 s*/
				unsigned char validate,	/* Validity date*/
				unsigned long tAcc,		/* Time accuracy ns UTC*/
				long nano,				/* Fraction of second, -1e9..1e9*/
				unsigned char fixType,	/* GNSS Fix Type 0=no fix, 1 deadreckoning only, 2=2D-fix, 3=3D-fix, 4=GNSS+dead reckoning, 5=time only*/
				unsigned char flags,	/* Fix status flag b0=valid fix, b1=diff corrections, b5=vehicule heading valid, b6..b7 carrSoln*/
				unsigned char flags2,	/* Additional flags (time validations)*/
				unsigned char numSV,	/* Number of satellites used in Nav Solution*/
				long lon,				/* Longitude in ie-7 deg */
				long lat,				/* Latitude in 1e-7 deg */
				long height,			/* Height above ellipsoid in mm */
				long hMSL,				/* Height above mean sea level mm */
				unsigned long hAcc,		/* Horizontal accuracy estimate mm */
				unsigned long vAcc,		/* Vertical accuracy estimate mm */
				long velN,				/* NED north velociy mm/s */
                long velE,				/* NED east velocity mm/s */
                long velD,				/* NED down velocity mm/s */
                long gSpeed,			/* Ground speed 2D mm/s */
                long headMot,			/* Heading of motion 2-D 1e-5 deg */
                unsigned long sAcc,		/* Speed accury estimate mm/s */
                unsigned long headAcc,	/* Heading accuracy estimate both motion & vehicule 1e-5 deg/s */
				unsigned short pDOP,	/* Position DOP 0.01 */
				unsigned char flags3,	/* Additional flags b0 invalid lon, lat, height & hMSL */
				unsigned char reserved01,/* Reserved*/
				unsigned long reserved02,/* Reserved */
				long headVeh,			/* Heading of vehicue 2-D */
				short magDec,			/* Magnetic declination 1e-2 deg */
                unsigned short magAcc) /* Magnetic declination accuracy 1e-2 deg */
	{
    if((fixType==3) || (fixType==4)) 	/* Si les données GPS sont valides */
    	gps_nav_valid = 1;
    	dead_reckon_clock = 26; 		/* il faudrait faire revenir DR_PERIOD ici */
    	Vsz_gps=-velD*0.001f;
    	Ground_Speed_gps = gSpeed*0.001f;
        time_gps = iTOW*0.001f;
        latitude = lat*1e-7f;
        longitude = lon*1e-7f;
        gps_altitude = hMSL*0.001f;
        date_gps = day;
    }
    else{/* Just to recognize bad parsing*/
    	gps_nav_valid = -1;
    	Vsz_gps=1.23;
        Ground_Speed_gps = 3.45;
        time_gps=0.0;
    }

}

void reportUnhandled(char msgid) { }


        /**
          * Constructs a UBX parser. 
          */
void Init_UBX_Parser()
        {
            state    = GOT_NONE;
            msgclass = -1;
            msgid    = -1;
            msglen   = -1;
            chka     = -1;
            chkb     = -1;
            count    = 0;
            gps_nav_valid = 0;
        };


        /**
          * Parses a new byte from the GPS. Automatically calls handle_ methods when a new
          * message is successfully parsed.
          * @param b the byte
          */
void parse(char b)
        {
            if (b == 0xB5) {

                state = GOT_SYNC1;
            }

            else if (b == 0x62 && state == GOT_SYNC1) {

                state = GOT_SYNC2;
                chka = 0;
                chkb = 0;
            }

            else if (state == GOT_SYNC2) {

                state = GOT_CLASS;
                msgclass = b;
                addchk(b);
            }

            else if (state == GOT_CLASS) {

                state = GOT_ID;
                msgid = b;
                addchk(b);
            }

            else if (state == GOT_ID) {

                state = GOT_LENGTH1;
                msglen = b;
                addchk(b);
            }

            else if (state == GOT_LENGTH1) {

                state = GOT_LENGTH2;
                msglen += (b << 8);
                count = 0;
                addchk(b);
            }

            else if (state == GOT_LENGTH2) {

                addchk(b);
                payload[count] = b;
                count += 1;

                if (count == msglen) {

                    state = GOT_PAYLOAD;
                }
            }

            else if (state == GOT_PAYLOAD) {

                state = (b == chka) ? GOT_CHKA : GOT_NONE;
            }

            else if (state == GOT_CHKA) {

                if (b == chkb) {
                    dispatchMessage();
                }

                else {
                    state = GOT_NONE;
                }
            }
    }

