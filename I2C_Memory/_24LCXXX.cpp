/**
 *****************************************************************************
 * File Name    : _24LCXXX.cpp
 *
 * Title        : I2C EEPROM 24LCXXX Claass Source File
 * Revision     : 0.1
 * Notes        :
 * Target Board : mbed NXP LPC1768, mbed LPC1114FN28  etc
 * Tool Chain   : ????
 *
 * Revision History:
 * When         Who                 Description of change
 * -----------  ------------------- -----------------------
 * 2015/04      Akafugu Corporation Fixing page write, part 2
 * 2015/02/26   Akafugu Corporation Fixing page write
 * 2012/12/06   Hiroshi M           init
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

/* Includes ----------------------------------------------------------------- */
#include "_24LCXXX.h"
#include "mbed.h"

/* Private typedef ---------------------------------------------------------- */
/* Private define ----------------------------------------------------------- */
#define MIN(a,b) ((a) < (b) ? (a) : (b))
/* Private macro ------------------------------------------------------------ */
/* Private variables -------------------------------------------------------- */

/* member functions --------------------------------------------------------- */

// Constructor
_24LCXXX::_24LCXXX(I2C *i2c, const int address):
    _i2c_address(address<<1), _i2c(i2c), _pc(NULL), _debug(false)
{
}

_24LCXXX::_24LCXXX(I2C *i2c, Serial *pc, const int address):
    _i2c_address(address<<1), _i2c(i2c), _pc(pc), _debug(true)
{
}

int _24LCXXX::nbyte_read( int mem_addr, void *data, int size )
{
    int res;
    char buf[2];

    buf[0] = (0xff00 & mem_addr)>>8;        // Read Address High byte set
    buf[1] = (0x00ff & mem_addr);           // Read Address Low  byte set

    res = _i2c->write(_i2c_address, buf, sizeof(buf), true);
    if(_debug)
    {
        if(res==0)
        {
            _pc->printf("Success! nbyte read address send. \n");
        }
        else
        {
            _pc->printf("Failed! nbyte read address send %d.\n", res);
        }
    }

    //
    res = _i2c->read(_i2c_address, (char *)data, size, false);
    if(_debug)
    {
        if(res==0)
        {
            _pc->printf("Success! nbyte read address send. \n");
        }
        else
        {
            _pc->printf("Failed! nbyte read address send %d.\n", res);
        }
    }

    return res;
}

int _24LCXXX::byte_write( int mem_addr, char data )
{
    int res;
    char buf[3];

    buf[0] = (0xff00 & mem_addr)>>8;        // Write Address High byte set
    buf[1] = (0x00ff & mem_addr);           // Write Address Low  byte set
    buf[2] = data;

    res = _i2c->write(_i2c_address, buf, sizeof(buf), false);
    if(_debug)
    {
        if(res==0)
        {
            _pc->printf("Success! Byte Data Write. \n");
        }
        else
        {
            _pc->printf("Failed! Byte Data Write %d.\n", res);
        }
    }

    wait_ms(5);         // 5mS

    return res;
}

int _24LCXXX::nbyte_write( int mem_addr, char *data, int size )
{
    return page_block(mem_addr, data, size, true);
}

int _24LCXXX::nbyte_set( int mem_addr, char data, int size )
{
    char buffer[PAGE_SIZE_24LCXXX];
    for (uint8_t i=0; i< PAGE_SIZE_24LCXXX; i++) buffer[i] = data;

    return page_block(mem_addr, buffer, size, false);
}


/* Private functions -------------------------------------------------------- */

// page_block aligns buffer to page boundaries for writing.
int _24LCXXX::page_block( int mem_addr, char *data, uint16_t length, bool incrBuffer )
{
    int res = 0;
    while (length > 0)
    {
        uint8_t bytesUntilPageBoundary = PAGE_SIZE_24LCXXX - mem_addr%PAGE_SIZE_24LCXXX;
        uint8_t cnt = MIN(length, bytesUntilPageBoundary);

        res = page_write(mem_addr, data, cnt); // todo check return value..
        if (res != 0) return res;

        mem_addr += cnt;
        if (incrBuffer) data += cnt;
        length -= cnt;
    }
    return res;
}

int _24LCXXX::page_write( int mem_addr, char *data, uint16_t length)
{
    int res;
    _i2c->start();
    _i2c->write(_i2c_address);
    _i2c->write((0xff00 & mem_addr)>>8);        // Write Address High byte set
    _i2c->write(0x00ff & mem_addr);           // Write Address Low  byte set
    for(int i = 0; i < length; i++)
    {
        res = !_i2c->write(data[i]); // return value from write needs to be inverted.
        if(res != 0) break; // Write unsuccessfull
    }

    _i2c->stop();

    if(_debug)
    {
        if(res==0)
        {
            _pc->printf("Success! Page Data Write. \n");
        }
        else
        {
            _pc->printf("Failed! Page Data Write %d.\n", res);
        }
    }

    wait_ms(5);         // 5mS

    return res;
}
