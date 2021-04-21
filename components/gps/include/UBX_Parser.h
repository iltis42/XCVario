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
extern float Vsz_gps;
void addchk(int b);
void dispatchMessage();
long unpack_int32(int offset);
long unpack_int16(int offset);
long unpack(int offset, int size);
void handle_NAV_POSLLH(unsigned long iTOW,
                long lon, 
                long lat, 
                long height, 
                long hMSL, 
                unsigned long hAcc, 
                unsigned long vAcc);
void handle_NAV_DOP(unsigned long iTOW,
                unsigned short gDOP,
                unsigned short pDOP,
                unsigned short tDOP,
                unsigned short vDOP,
                unsigned short hDOP,
                unsigned short nDOP,
                unsigned short eDOP);
void handle_NAV_VELNED(unsigned long iTOW,
                long velN,
                long velE,
                long velD,
                unsigned long speed,
                unsigned long gSpeed,
                long heading,
                unsigned long sAcc,
                unsigned long cAcc);
/*virtual void handle_NAV_SOL(unsigned long iTOW,
                 long fTOW, 
                 short week, 
                 char gpsFix) { }
                 */

void reportUnhandled(char msgid);
void Init_UBX_Parser();
void parse(int b);
