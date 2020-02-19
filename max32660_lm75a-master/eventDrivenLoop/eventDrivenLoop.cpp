/*
** file name  : eventDrivenLoop.cpp
** description: 
**		- original: This code is provided as an example only and is not supported or guaranteed by FTDI
**                  It is the responsibility of the recipient to ensure correct operation of their overall system
**		- changes : As a I2C client, wait data from the master and send it back as received. Event is created when 
**                  a char received, then wake up for receiving all succeeding characters and feed them back to the board 
**                  hence to the I2C master.
*/
#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include "ftd2xx.h"

typedef struct {
	unsigned : 3;	//-- reserved [7:5]
	unsigned FAULT_QUEUE : 2;	//-- FAULT_QUEUE[2]
	unsigned OSPOLARITY : 1;	//-- alert polarity (1:active high, 0: active low)
	unsigned MODE : 1;			//-- mode (Cmp/Int) (1:interrupt mode, 0: comparator)
	unsigned SHUTDOWN : 1;		//-- shutdown -> low power shutdown mode
}	lm75a_cfgbits_t;

typedef union {
	lm75a_cfgbits_t		cfgbits;
	unsigned char		cfgchar;
}	lm75a_cfg_t;
lm75a_cfg_t lm75a_cfg;

#define LM75A_REG_TEMP		0x00
#define LM75A_REG_CFG		0x01
#define LM75A_REG_T_HYST	0x02
#define LM75A_REG_T_OS		0x03
#define LM75A_REG_ID		0x07

#pragma comment(lib, "ftd2xx.lib")	//-- This is required to include the library ftd2xx.lib

int main(int argc, char* argv[])
{
	FT_HANDLE	fthandle;	
	FT_STATUS	status;
	HANDLE		hEvent;
	DWORD		EventMask;
	DWORD		numdev = 0;
	LONG		lComPortNumber;
	DWORD		EventDWord;
	DWORD		RxBytes, TxBytes;
	char		data_in[16], data_out[16];
	char		TLSB, TMSB;
	DWORD		BytesReceived = 0;
	DWORD		SoftwareTimeout = 0;
	FT_DEVICE_LIST_INFO_NODE *devInfo;

	//-- LM75A Configuration -------------------------------------------
	lm75a_cfg.cfgbits.FAULT_QUEUE = 0;
	lm75a_cfg.cfgbits.OSPOLARITY = 1;
	lm75a_cfg.cfgbits.MODE = 1;
	lm75a_cfg.cfgbits.SHUTDOWN = 1;

	//-- Check how many FTDI devices are connected and installed. If one or more connected, open the first one
	status = FT_CreateDeviceInfoList(&numdev);
	if 	((status == FT_OK) && (numdev > 0) ) {
		//-- get device detail information
		devInfo = (FT_DEVICE_LIST_INFO_NODE*)malloc(sizeof(FT_DEVICE_LIST_INFO_NODE)*numdev);
		status = FT_GetDeviceInfoList(devInfo, &numdev);
		if (status == FT_OK) {
			for (int i = 0; i < (int)numdev; i++) {
				printf("Dev %d:\n", i);
				printf("\tFlags=0x%x\n", devInfo[i].Flags);
				printf("\tType=0x%x\n", devInfo[i].Type);
				printf("\tID=0x%x\n", devInfo[i].ID);
				printf("\tLocId=0x%x\n", devInfo[i].LocId);
				printf("\tSerialNumber=%s\n", devInfo[i].SerialNumber);
				printf("\tDescription=%s\n", devInfo[i].Description);
				// printf("\tftHandle=0x%x\n", devInfo[i].ftHandle);
			}
		}

		//-- Open the device now -----------------------------------------
		status = FT_Open(0, &fthandle);
		if(status != FT_OK) 
			printf("status not ok %d\n", status);

		status = FT_GetComPortNumber(fthandle, &lComPortNumber);
		if (status != FT_OK)
			printf("status not ok %d\n", status);
		else
			printf("COM port number : COM%02d\n", lComPortNumber);

		//-- sets the read and write timeouts for the device ------------
		status = FT_SetTimeouts(fthandle,
			5000,	//-- dwReadTimeout
			5000	//-- dwWriteTimeout
		);
		if (status != FT_OK)
			printf("status not ok %d\n", status);

		//---------------------------------------------------------------------------
		//-- setup for the event driven processing
		//---------------------------------------------------------------------------
		hEvent = CreateEvent(NULL,
			false, /* auto-reset event */
			false, /* non-signaled state */
			NULL);
		EventMask = FT_EVENT_RXCHAR | FT_EVENT_MODEM_STATUS;	//-- received char
		status = FT_SetEventNotification(fthandle, EventMask, hEvent);

		//-- Sometime later, block the application thread by waiting on the event,
		//-- then when the event has occurred, determine the condition that caused the event,
		//-- and process it accordingly.
		TMSB = 0x0D; TLSB = 0x01;
		while (1) {
			printf("==> Waiting for event! Start I2C on Max32660 now!\n");
			WaitForSingleObject(hEvent, INFINITE);
			FT_GetStatus(fthandle, &RxBytes, &TxBytes, &EventDWord);
			printf("* Got RxBytes %d, TxBytes, %d\n", RxBytes, TxBytes);

			status = FT_Read(fthandle, data_in, RxBytes, &BytesReceived);
			//-- need to verify the bytes we read
			// for (DWORD i = 0; i < BytesReceived; i++)
				printf("* Got command : 0x%02x\n", data_in[0]);
			// printf("\n");

			//-- I am simulating LM75A temperature sensor
			DWORD w_data_len;
			DWORD data_written;

			switch (data_in[0]) {
			case LM75A_REG_TEMP:	//-- 0x00, requesting temperature
				w_data_len = 2;
				data_out[0] = TMSB;
				data_out[1] = TLSB;
				printf("\tLM75A : writing temperature : 0x%02x, 0x%02x, %d ", TMSB, TLSB, (TMSB<<8|TLSB));
				break;
			case LM75A_REG_CFG:		//-- 0x01, configuration 
				w_data_len = 1;
				data_out[0] = lm75a_cfg.cfgchar;
				printf("\tLM75A : writing configuration :0x%02x\n", data_out[0]);
				break;
			case LM75A_REG_T_HYST:	//-- 0x02, Thyst = 75 Deg C 
				w_data_len = 1;
				data_out[0] = 75;
				printf("\tLM75A : writing Thyst :0x%02x\n", data_out[0]);
				break;
			case LM75A_REG_T_OS:	//-- 0x03, Tos   = 80 Deg C
				w_data_len = 1;
				data_out[0] = 80;
				printf("\tLM75A : writing Tos :0x%x\n", data_out[0]);
				break;
			case LM75A_REG_ID:		//-- 0x07, ID
				w_data_len = 1;
				data_out[0] = 0xA1;
				printf("\tLM75A : writing board id:0x%x\n", data_out[0]);
				break;
			}
			//-- Now write some data to the chips buffer
			if (w_data_len > 0) {
				status = FT_Write(fthandle, data_out, w_data_len, &data_written);
				if (status != FT_OK)
					printf("\tstatus not ok %d\n", status);
				else
					printf("\tSent %d bytes!\n", data_written);
			}
		} //-- end of while
		//-- Close the device
		status = FT_Close(fthandle);
	} else {
		printf("No FTDI devices connected to the computer \n");		
	}
	
	printf("Press Return To End Program");
	getchar();
	printf("closed \n");
	return 0;
}