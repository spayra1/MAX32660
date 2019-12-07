/**
  EEPROM I2C Source File

  Company:
    Microchip Technology Inc.

  File Name:
    eeprom_i2c.c

  Summary:
   This is the source file containing the EEPROM I2C functions and constants.

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  MPLAB® Code Configurator - v2.25.2
        Device            :  PIC16F1719
        Driver Version    :  2.00
    The generated drivers are tested against the following:
        Compiler          :  XC8 v1.34
        MPLAB             :  MPLAB X v2.35 or v3.00
 */

/*
Copyright (c) 2013 - 2015 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 */
 

#include "eeprom_i2c.h"

uint8_t timeOut = 0;

int I2C_ByteWrite(uint8_t *dataAddress, uint8_t dataByte, uint8_t addlen)
{
   uint8_t writeBuffer[PAGE_LIMIT+3];
   uint8_t buflen;
   
   //Copy address bytes to the write buffer so it can be sent first 
    for(int i = 0; i < addlen; i++)
    {
        writeBuffer[i] = dataAddress[i];
    }
    
   //Check if this is an address write or a data write.
   if(dataByte != NULL)
   {
       writeBuffer[addlen] = dataByte;
       buflen = addlen+1;
   }        
   else
       buflen = addlen;
   
    //set status to Message Pending to send the data
    I2C_MESSAGE_STATUS status = I2C_MESSAGE_PENDING;
    
    //This variable is the built in acknowledge polling mechanism. This counts how many retries the system has already done to send the data.
    timeOut = 0;
    
    //While the message has not failed...
    while(status != I2C_MESSAGE_FAIL)
    {
        // Initiate a write to EEPROM
            I2C_MasterWrite(writeBuffer,buflen,SLAVE_ADDRESS,&status);

        // wait for the message to be sent or status has changed.
            while(status == I2C_MESSAGE_PENDING);
       // if transfer is complete, break the loop
            if (status == I2C_MESSAGE_COMPLETE)
                break;
                // if transfer fails, break the loop
            if (status == I2C_MESSAGE_FAIL)
                break;
        //Max retry is set for max Ack polling. If the Acknowledge bit is not set, this will just loop again until the write command is acknowledged
            if (timeOut == MAX_RETRY)
                break;
            else
                timeOut++;
    }
                // if the transfer failed, stop at this point
                if (status == I2C_MESSAGE_FAIL)
                return 1;
}

uint8_t I2C_ByteRead(uint8_t *dataAddress,uint8_t dataByte, uint8_t addlen)
{
    int check;
    
    //Write the address to the slave
    check = I2C_ByteWrite(dataAddress,NULL,addlen);
    
    //If not successful, return to function
    if(check == 1)
        return;
    
    //Get ready to send data
    I2C_MESSAGE_STATUS status = I2C_MESSAGE_PENDING;
    //Set up for ACK polling
    timeOut = 0;
    
    //While the code has not detected message failure..
    while(status != I2C_MESSAGE_FAIL)
    {      
        // Initiate a Read to EEPROM 
            I2C_MasterRead(dataByte,1,SLAVE_ADDRESS,&status);
            
        // wait for the message to be sent or status has changed.
            while(status == I2C_MESSAGE_PENDING);
            
        // if transfer is complete, break the loop
            if (status == I2C_MESSAGE_COMPLETE)
                break;
            
        // if transfer fails, break the loop
            if (status == I2C_MESSAGE_FAIL)
                break;
            
        // check for max retry and skip this byte
            if (timeOut == MAX_RETRY)
                break;
            else
                timeOut++;
    }
}
int I2C_BufferWrite(uint8_t *dataAddress, uint8_t *dataBuffer, uint8_t addlen, uint8_t buflen)
{
    uint8_t writeBuffer[PAGE_LIMIT+3];
    I2C_MESSAGE_STATUS status = I2C_MESSAGE_PENDING;
    
    //Set Address as the bytes to be written first
    for(int i = 0; i < addlen; i++)
    {
        writeBuffer[i] = dataAddress[i];
    }
    
    //Ensure that the page limit is not breached so as to avoid overwriting other data
    if(buflen > PAGE_LIMIT)
        buflen = PAGE_LIMIT;
    
    //Copy data bytes to write buffer
    for(int j = 0; j < buflen; j++)
    {
        writeBuffer[addlen+j] = dataBuffer[j];
    }
    //Set up for ACK polling
    timeOut = 0;
    
    while(status != I2C_MESSAGE_FAIL)
    {
        // Initiate a write to EEPROM
            I2C_MasterWrite(writeBuffer,buflen+addlen,SLAVE_ADDRESS,&status);

        // wait for the message to be sent or status has changed.
            while(status == I2C_MESSAGE_PENDING);
       // if transfer is complete, break the loop
            if (status == I2C_MESSAGE_COMPLETE)
                break;
                // if transfer fails, break the loop
            if (status == I2C_MESSAGE_FAIL)
                break;

        //check for max retry and skip this byte
            if (timeOut == MAX_RETRY)
                break;
            else
                timeOut++;
    }
                // if the transfer failed, stop at this point
                if (status == I2C_MESSAGE_FAIL)
                return 1;

}
void I2C_BufferRead(uint8_t *dataAddress, uint8_t *dataBuffer, uint8_t addlen,uint8_t buflen)
{
    int check = 0;
    I2C_MESSAGE_STATUS status = I2C_MESSAGE_PENDING;
    
    //Write Address from where to read
    check = I2C_ByteWrite(dataAddress,NULL,addlen);
    
    //check if address write is successful
    if(check == 1)
        return;
   
    //Set up for ACK polling
    timeOut = 0;
    
    while(status != I2C_MESSAGE_FAIL){      
        // Initiate a Read to EEPROM 
        I2C_MasterRead(dataBuffer,buflen,SLAVE_ADDRESS,&status);
            
        // wait for the message to be sent or status has changed.
        while(status == I2C_MESSAGE_PENDING);
        
        // if transfer is complete, break the loop
        if (status == I2C_MESSAGE_COMPLETE)
            break;
            
        // if transfer fails, break the loop
        if (status == I2C_MESSAGE_FAIL)
                break;
            
        // check for max retry and skip this byte
        if (timeOut == MAX_RETRY)
            break;
        else
                timeOut++;
        } 
}
