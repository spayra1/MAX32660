/*
 * i2c_flags.h
 *
 *  Created on: Jul 23, 2018
 *      Author: Cyberarea
 */

#ifndef I2C_CHECK_FLAGS_H_
#define I2C_CHECK_FLAGS_H_

#include "i2c.h"
#include "mxc_config.h"

//-- I2C0_CTRL0 : Control Register 0 ---------------------------------------------
#define I2C0_CTRL_MASK		0x0000FFFF	//-- [31:16] reserved
#define I2C0_CTRL_HSMODE	0x00008000	//-- [15]
#define I2C0_CTRL_SCL_PPM	0x00002000	//-- [13]
#define I2C0_CTRL_SCL_STRD	0x00001000	//-- [12]
#define I2C0_CTRL_READ		0x00000800	//-- [11]
#define I2C0_CTRL_SWOE		0x00000400	//-- [10]
#define I2C0_CTRL_SDA		0x00000200	//-- [9]
#define I2C0_CTRL_SCL		0x00000100	//-- [8]
#define I2C0_CTRL_SDAO		0x00000080	//-- [7]
#define I2C0_CTRL_SCLO		0x00000040	//-- [6]
#define I2C0_CTRL_ACK		0x00000010	//-- [4]
#define I2C0_CTRL_IRXM		0x00000008	//-- [3]
#define I2C0_CTRL_GCEN		0x00000004	//-- [2]
#define I2C0_CTRL_MST		0x00000002	//-- [1]
#define I2C0_CTRL_I2CEN		0x00000001	//-- [0]

//-- I2C0_STATUS : Status Register ------------------------------------------------
#define I2C0_STATUS_MASK	0x0000003F	//-- [31:6] reserved
#define I2C0_STATUS_CKMD	0x00000020	//-- [5]
#define I2C0_STATUS_TXF		0x00000010	//-- [4]
#define I2C0_STATUS_TXE		0x00000008	//-- [3]
#define I2C0_STATUS_RXF		0x00000004	//-- [2]
#define I2C0_STATUS_RXE		0x00000002	//-- [1]
#define I2C0_STATUS_BUSY	0x00000001	//-- [0]

//-- I2C0_INTFL0 : Interrupt Status Register --------------------------------------
#define I2C0_INTFL0_MASK	0x0000FFFF	//-- [31:16] reserved
#define I2C0_INTFL0_TXLOI	0x00008000	//-- [15]
#define I2C0_INTFL0_STOPERI	0x00004000	//-- [14]
#define	I2C0_INTFL0_STRTERI	0x00002000	//-- [13]
#define	I2C0_INTFL0_DNRERI	0x00001000	//-- [12]
#define I2C0_INTFL0_DATERI	0x00000800	//-- [11]
#define I2C0_INTFL0_ADRERI	0x00000400	//-- [10]
#define I2C0_INTFL0_TOERI	0x00000200	//-- [9]
#define I2C0_INTFL0_ARBERI	0x00000100	//-- [8]
#define I2C0_INTFL0_ADRACKI	0x00000080	//-- [7]
#define I2C0_INTFL0_STOPI	0x00000040	//-- [6]
#define I2C0_INTFL0_TXTHI	0x00000020	//-- [5]
#define I2C0_INTFL0_RXTHI	0x00000010	//-- [4]
#define I2C0_INTFL0_AMI		0x00000008	//-- [3]
#define I2C0_INTFL0_GCI		0x00000004	//-- [2]
#define I2C0_INTFL0_IRXMI	0x00000002	//-- [1]
#define I2C0_INTFL0_DONEI	0x00000001	//-- [0]

//-- I2C0_INTEN0 : Interrupt Enable Register --------------------------------------
#define I2C0_INTEN0_MASK	0x0000FFFF	//-- [31:16] reserved
#define I2C0_INTEN0_TXLOIE	0x00008000	//-- [15] R/W, TX FIFO Locked Out Interrupt Enable
#define I2C0_INTEN0_STOPERIE 0x00004000 //-- [14] R/W, Out of Sequence STOP Condition Detected Interrupt Enable
#define I2C0_INTEN0_STRTERIE 0x00002000 //-- [13] R/W, Out of Sequence START condition detected Interrupt Enable
#define I2C0_INTEN0_DNRERIE	0x00001000	//-- [12] R/W, Slave Mode Do Not Respond Interrupt Enable
#define I2C0_INTEN0_DATERIE	0x00000800	//-- [11] R/W, Master Mode Received Data NACK from Slave Interrupt Enable
#define I2C0_INTEN0_ADRERIE 0x00000400	//-- [10] R/W, master Received Address NACK from SLave Interrupt Enable
#define I2C0_INTEN0_TOERIE 	0x00000200	//-- [9]  R/W, Timeout Error Interrupt Enabled
#define I2C0_INTEN0_ARBERIE 0x00000100	//-- [8]  R/W, Master Mode Arbitration Lost Interrupt Enable
#define I2C0_INTEN0_ADRACKIE 0x0000080	//-- [7]  R/W, Received Address ACK from Slave Interrupt Enabled
#define I2C0_INTEN0_STOPIE	0x00000040	//-- [6]  R/W, STOP Condition Detected Interrupt Enable
#define I2C0_INTEN0_TXTHIE	0x00000020 	//-- [5]  R/W, TX FIFO Threshold Level Interrupt Enable
#define I2C0_INTEN0_RXTHIE	0x00000010  //-- [4]  R/W, RX FIFO Threshold Level Interrupt Enable
#define I2C0_INTEN0_AMIE	0x00000008	//-- [3]  R/W, Slave Mode Incoming Address Match Interrupt Enable
#define I2C0_INTEN0_GCIE	0x00000004 	//-- [2]  R/W, Slave Mode General Call Address Match Received Interrupt Enable
#define I2C0_INTEN0_IRXMIE	0x00000002 	//-- [1]  R/W, Interactive Receive Interrupt Enable
#define I2C0_INTEN0_DONEIE  0x00000001  //-- [0]  R/W, Tranfser Complete Interrupt Enable

//-- I2C0_RX_CTRL : Receive Control Register 0 ------------------------------------
#define I2C0_RXCTRL_MASK	0x0000FFFF

//-- I2C0_RX_CTRL : Transmit Control Register 0 -----------------------------------
#define I2C0_TXCTRL_MASK	0x0000FFFF

//-- I2C_FIFO : FIFO Data Register

//-- function prototypes ----------------------------------------------------------
void i2c0_status_check(void);
void i2c0_interrupt_check(void);

#endif /* I2C_CHECK_FLAGS_H_ */
//-- end of program ---------------------------------------------------------------
