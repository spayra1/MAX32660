/**
 *****************************************************************************
 * File Name    : _24LCXXX.h
 *
 * Title        : I2C EEPROM 24LCXXX Claass Header File
 * Revision     : 0.1
 * Notes        :
 * Target Board : mbed NXP LPC1768, mbed LPC1114FN28  etc
 * Tool Chain   : ????
 *
 * Revision History:
 * When         Who         Description of change
 * -----------  ----------- -----------------------
 * 2015/02/26   Akafugu     Fixing page write
 * 2012/12/06   Hiroshi M   init
 *****************************************************************************
 *
 * Copyright (C) 2013 Hiroshi M, MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 **/

#ifndef __24LCXXX_H_
#define __24LCXXX_H_

/* Includes ----------------------------------------------------------------- */
#include "mbed.h"

/* typedef ------------------------------------------------------------------ */

/* define ------------------------------------------------------------------- */
#define     I2C_ADDR_24LCXXX        0x50
#define     __24LC512__

#if defined(__24LC64__)
#define     MAXADR_24LCXXX          8192
#define     PAGE_SIZE_24LCXXX       32
#endif

#if defined(__24LC128__)
#define     MAXADR_24LCXXX          16384
#define     PAGE_SIZE_24LCXXX       64
#endif

#if defined(__24LC256__)
#define     MAXADR_24LCXXX          32768
#define     PAGE_SIZE_24LCXXX       64
#endif

#if defined(__24LC512__)
#define     MAXADR_24LCXXX          65536
#define     PAGE_SIZE_24LCXXX       128
#endif

/* macro -------------------------------------------------------------------- */
/* variables ---------------------------------------------------------------- */
/* class -------------------------------------------------------------------- */

class _24LCXXX
{
public:
    _24LCXXX(I2C *i2c, const int address=I2C_ADDR_24LCXXX );
    _24LCXXX(I2C *i2c, Serial *pc, const int address=I2C_ADDR_24LCXXX );
    int nbyte_read( int mem_addr, void *data, int size );
    int byte_write( int mem_addr, char data );
    int nbyte_write( int mem_addr, char *data, int size );
    int nbyte_set( int mem_addr, char data, int size );

private:
    int _i2c_address;
    I2C *_i2c;
    Serial *_pc;
    bool _debug;

    int page_block( int mem_addr, char *data, uint16_t length, bool incrBuffer );
    int page_write( int mem_addr, char *data, uint16_t length);
};

#endif /* __24LCXXX_H_ */

