/*

  ShowTGA.ino
  
  Show TGA file with Ucglib

  Universal uC Color Graphics Library
  
  Copyright (c) 2014, olikraus@gmail.com
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, 
  are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list 
    of conditions and the following disclaimer.
    
  * Redistributions in binary form must reproduce the above copyright notice, this 
    list of conditions and the following disclaimer in the documentation and/or other 
    materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  

*/

#include <SPI.h>
#include <SD.h>
#include "Ucglib.h"

    
/*
  Hardware SPI Pins:
    Arduino Uno		sclk=13, data=11
    Arduino Due		sclk=76, data=75
    Arduino Mega	sclk=52, data=51
    
  >>> Please uncomment (and update) one of the following constructors. <<<  
*/
//Ucglib8BitPortD ucg(ucg_dev_ili9325_18x240x320_itdb02, ucg_ext_ili9325_18, /* wr= */ 18 , /* cd= */ 19 , /* cs= */ 17, /* reset= */ 16 );
//Ucglib8Bit ucg(ucg_dev_ili9325_18x240x320_itdb02, ucg_ext_ili9325_18, 0, 1, 2, 3, 4, 5, 6, 7, /* wr= */ 18 , /* cd= */ 19 , /* cs= */ 17, /* reset= */ 16 );

//Ucglib4WireSWSPI ucg(ucg_dev_ili9325_18x240x320_itdb02, ucg_ext_ili9325_18, /*sclk=*/ 13, /*data=*/ 11, /*cd=*/ 9 , /*cs=*/ 10, /*reset=*/ 8);	// not working
//Ucglib4WireSWSPI ucg(ucg_dev_ili9325_spi_18x240x320, ucg_ext_ili9325_spi_18, /*sclk=*/ 13, /*data=*/ 11, /*cd=*/ 9 , /*cs=*/ 10, /*reset=*/ 8);	// not working
//Ucglib3WireILI9325SWSPI ucg(ucg_dev_ili9325_spi_18x240x320, ucg_ext_ili9325_spi_18, /*sclk=*/ 13, /*data=*/ 11, /*cs=*/ 10, /*reset=*/ 8);	// not working
//Ucglib3WireILI9325SWSPI ucg(ucg_dev_ili9325_18x240x320_itdb02, ucg_ext_ili9325_18, /*sclk=*/ 13, /*data=*/ 11, /*cs=*/ 10, /*reset=*/ 8);	// not working

//Ucglib_ST7735_18x128x160_SWSPI ucg(/*sclk=*/ 13, /*data=*/ 11, /*cd=*/ 9, /*cs=*/ 10, /*reset=*/ 8);
//Ucglib_ST7735_18x128x160_HWSPI ucg(/*cd=*/ 9, /*cs=*/ 10, /*reset=*/ 8);

//Ucglib_ILI9163_18x128x128_SWSPI ucg(/*sclk=*/ 7, /*data=*/ 6, /*cd=*/ 5, /*cs=*/ 3, /*reset=*/ 4);
//Ucglib_ILI9163_18x128x128_HWSPI ucg(/*cd=*/ 9, /*cs=*/ 10, /*reset=*/ 8);	/* HW SPI Adapter */

//Ucglib_ILI9341_18x240x320_SWSPI ucg(/*sclk=*/ 7, /*data=*/ 6, /*cd=*/ 5, /*cs=*/ 3, /*reset=*/ 4);
//Ucglib_ILI9341_18x240x320_SWSPI ucg(/*sclk=*/ 13, /*data=*/ 11, /*cd=*/ 9, /*cs=*/ 10, /*reset=*/ 8);
//Ucglib_ILI9341_18x240x320_HWSPI ucg(/*cd=*/ 9, /*cs=*/ 10, /*reset=*/ 8);
//Ucglib_ILI9341_18x240x320_SWSPI ucg(/*sclk=*/ 4, /*data=*/ 3, /*cd=*/ 6, /*cs=*/ 7, /*reset=*/ 5);	/* Elec Freaks Shield */

//Ucglib_HX8352C_18x240x400_SWSPI ucg(/*sclk=*/ 7, /*data=*/ 6, /*cd=*/ 5, /*cs=*/ 3, /*reset=*/ 4);
//Ucglib_HX8352C_18x240x400_HWSPI ucg(/*cd=*/ 9, /*cs=*/ 10, /*reset=*/ 8);

//Ucglib_ILI9486_18x320x480_SWSPI ucg(/*sclk=*/ 7, /*data=*/ 6, /*cd=*/ 5, /*cs=*/ 3, /*reset=*/ 4);
//Ucglib_ILI9486_18x320x480_HWSPI ucg(/*cd=*/ 9, /*cs=*/ 10, /*reset=*/ 8);

//Ucglib_SSD1351_18x128x128_SWSPI ucg(/*sclk=*/ 13, /*data=*/ 11, /*cd=*/ 9, /*cs=*/ 10, /*reset=*/ 8);
//Ucglib_SSD1351_18x128x128_HWSPI ucg(/*cd=*/ 9, /*cs=*/ 10, /*reset=*/ 8);
//Ucglib_SSD1351_18x128x128_FT_SWSPI ucg(/*sclk=*/ 13, /*data=*/ 11, /*cd=*/ 9, /*cs=*/ 10, /*reset=*/ 8);
//Ucglib_SSD1351_18x128x128_FT_HWSPI ucg(/*cd=*/ 9, /*cs=*/ 10, /*reset=*/ 8);

//Ucglib_PCF8833_16x132x132_SWSPI ucg(/*sclk=*/ 13, /*data=*/ 11, /*cs=*/ 9, /*reset=*/ 8);	/* linksprite board */
//Ucglib_PCF8833_16x132x132_HWSPI ucg(/*cs=*/ 9, /*reset=*/ 8);	/* linksprite board */

//Ucglib_LD50T6160_18x160x128_6Bit ucg( /*d0 =*/ d0, /*d1 =*/ d1, /*d2 =*/ d2, /*d3 =*/ d3, /*d4 =*/ d4, /*d5 =*/ d5, /*wr=*/ wr, /*cd=*/ cd, /*cs=*/ cs, /*reset=*/ reset);
//Ucglib_LD50T6160_18x160x128_6Bit ucg( /*d0 =*/ 16, /*d1 =*/ 17, /*d2 =*/ 18, /*d3 =*/ 19, /*d4 =*/ 20, /*d5 =*/ 21, /*wr=*/ 14, /*cd=*/ 15); /* Samsung 160x128 OLED with 6Bit minimal interface with Due */
//Ucglib_LD50T6160_18x160x128_6Bit ucg( /*d0 =*/ 5, /*d1 =*/ 4, /*d2 =*/ 3, /*d3 =*/ 2, /*d4 =*/ 1, /*d5 =*/ 0, /*wr=*/ 7, /*cd=*/ 6); /* Samsung 160x128 OLED with 6Bit minimal interface with Uno */

//Ucglib_SSD1331_18x96x64_UNIVISION_SWSPI ucg(/*sclk=*/ 13, /*data=*/ 11, /*cd=*/ 9, /*cs=*/ 10, /*reset=*/ 8);
//Ucglib_SSD1331_18x96x64_UNIVISION_HWSPI ucg(/*cd=*/ 9, /*cs=*/ 10, /*reset=*/ 8);

//Ucglib_SEPS225_16x128x128_UNIVISION_SWSPI ucg(/*sclk=*/ 13, /*data=*/ 11, /*cd=*/ 9, /*cs=*/ 10, /*reset=*/ 8);
//Ucglib_SEPS225_16x128x128_UNIVISION_HWSPI ucg(/*cd=*/ 9, /*cs=*/ 10, /*reset=*/ 8);


void setup(void)
{
  delay(1000);
  ucg.begin(UCG_FONT_MODE_TRANSPARENT);
  //ucg.begin(UCG_FONT_MODE_SOLID);
  ucg.clearScreen();
}

ucg_t *ucg_ptr;

const int sd_chip_select = 10; 

File tga_file;
uint8_t tga_read_byte(void)
{
  return tga_file.read();
}

void tga_read_buf(uint8_t *buf, uint16_t len)
{
  uint16_t i;
  for( i = 0; i < len; i++ )
    buf[i] = tga_read_byte();
}

uint8_t tga_open(const char *name)
{
  tga_file = SD.open(name, O_RDONLY);
  if ( tga_file )
    return 1;
  return 0;
}

void tga_close(void)
{
  if ( tga_file )
  {
    tga_file.close();
  }
}


uint16_t tga_width;
uint16_t tga_height;
uint16_t tga_x;
uint16_t tga_y;

uint16_t tga_read_word(void)
{
  uint16_t v;
  v = tga_read_byte();
  v |= tga_read_byte() << 8;
  return v;
}


/* read header as generated by gimp without RLE */
uint8_t tga_read_header(void)
{
  uint8_t v, i;
  tga_x = 0;                    /* reset draw position */
  tga_y = 0;
  v = tga_read_byte();		/* Imgae ID field, must be 0 */
  if ( v != 0 ) return 0;
  v = tga_read_byte();		/* Color map type, must be 0 */
  if ( v != 0 ) return 0;
  v = tga_read_byte();		/* Image type, must be 2, uncompressed true color */
  if ( v != 2 ) return 0;
  for( i = 0; i < 9; i++ )
    tga_read_byte();			/* skip color map and origin */
  tga_width = tga_read_word();
  tga_height = tga_read_word();
  v = tga_read_byte();		/* bits per pixel, must be 24, true color */
  if ( v != 24 ) return 0;
  v = tga_read_byte();		/* image descriptor, must be 0, no alpha */
  if ( v != 0 ) return 0;
  return 1;
}

#define TGA_PIX_BUF_SIZE 64
uint8_t tga_pixel[3*TGA_PIX_BUF_SIZE];
uint8_t tga_read_write_pixel(void)
{
  uint8_t i;
  uint8_t *buf;

    
  buf = tga_pixel;
  tga_read_buf(tga_pixel, 3*TGA_PIX_BUF_SIZE);
  for( i = 0; i < TGA_PIX_BUF_SIZE; i++ )
  {
    ucg_SetColor(ucg_ptr, 0, buf[2], buf[1], buf[0]);
    buf += 3;
    ucg_DrawPixel(ucg_ptr,tga_x, ucg_GetHeight(ucg_ptr)-1-tga_y);
    tga_x++;
    if ( tga_x >= tga_width )
    {
      tga_x = 0;
      tga_y++;
      if ( tga_y > tga_height )
        return 0;
    }
  }
  return 1;
}


uint8_t pos = 0;



void draw(void) 
{
  if ( tga_open("test.tga") != 0 )
  {
    if ( tga_read_header() != 0 )
    {
      while( tga_read_write_pixel() != 0 )
        ;
    }
    tga_close();
  }
}

ucg_t ucg_obj;

void loop(void)
{
  ucg_ptr = ucg.getUcg();  
  draw();
  delay(500);  
  
}


