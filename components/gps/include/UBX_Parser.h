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
#ifdef __cplusplus
extern "C"
{
#endif
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
extern float Ground_Speed_gps;
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

void handle_NAV_PVT(unsigned long iTOW,
		        unsigned short year,
				unsigned char month,
				unsigned char day,
				unsigned char hour,
				unsigned char minute,
				unsigned char seconde,
				unsigned char validate,
				unsigned long tAcc,
				long nano,
				unsigned char fixType,
				unsigned char flags,
				unsigned char flags2,
				unsigned char numSV,
				long lon,
				long lat,
				long height,
				long hMSL,
				unsigned long hAcc,
				unsigned long vAcc,
				long velN,
                long velE,
                long velD,
                long gSpeed,
                long headMot,
                unsigned long sAcc,
                unsigned long headAcc,
				unsigned short pDOP,
				unsigned char flags3,
				unsigned short reserved01,
				unsigned long reserved02,
				long headVeh,
				short magDec,
                unsigned short magAcc);

void reportUnhandled(char msgid);
void Init_UBX_Parser();
void parse(int b);

#ifdef __cplusplus
}
#endif
