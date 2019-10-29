

#ifndef BITBANG_I2C
#define BITBANG_I2C

#include <Arduino.h>

#define RISE_OR_FALL_FLANK_OK		0
#define FALL_FLANK_TIMEOUT_ERROR	20
#define RISE_FLANK_TIMEOUT_ERROR	21
#define SLAVE_NACK			22
#define SLAVE_DIDNT_NACK		23


#define COMM_FOUND		50
#define COMM_NOT_FOUND		49
#define END_COMM		48

#define STOP_TIMEOUT    0xFFF

void i2cPinesDefine(unsigned int sda_Pin, unsigned int scl_Pin);

// #######################################   SLAVE   FUNCTIONS   ###########################################################

// ******************* SDA, SCL SLAVE CONTROL ********************************* 
void i2cSlaveHighSda(void);

void i2cSlaveLowSda(void);

void i2cSlaveHighScl(void);

// ************************** Copying data to the othres sda's ports ****************

void copySda(bool d, unsigned int sdaP);

void i2cSlaveSdasLow(unsigned int* sdaPines);

// ************************** Reading data from the others sda's ports **********************



// ******************* SLAVE NEEDS TO READ THE RISE AND FALL SCL/SDA FLANK *********************************

//Waits a rise flank in the port PIN, while a given timeout
unsigned int waitRiseFlank(unsigned char PIN, unsigned long timeout);

//Waits a rise flank in the port PIN, while is copying the sdaPin (connected to master) signal to sda port (connected to slave), all this while a given timeout 
unsigned int waitRiseFlankCopyingOne(unsigned char sclPIN, unsigned long timeout, unsigned int sdaToRead, unsigned int sdaToWrite);

//Waits a rise flank in the port PIN, while is copying the sdaPin (connected to master) signal to all ports from sdaPines array ports(connected to slaves), all this while a given timeout
unsigned int waitRiseFlankCopyingAll(unsigned char PIN, unsigned long timeout, unsigned int* sdaPines);

//Waits a fall flank in the port PIN, while a given timeout
unsigned int waitFallFlank(unsigned char PIN, unsigned long timeout);

//Waits a fall flank in the port PIN, while is copying the sdaPin (connected to master) signal to sda port (connected to slave), all this while a given timeout 
unsigned int waitFallFlankCopyingOne(unsigned char sclPIN, unsigned long timeout, unsigned int sdaToRead, unsigned int sdaToWrite);

//This function waits a fall flank, coping the data in sdaPin to the othres sdaPines.
unsigned int waitFallFlankCopyingAll(unsigned char PIN, unsigned long timeout, unsigned int* sdaPines);

// ******************* READ AND WRITE BYTE SLAVE FUNCTIOINS  *********************************

unsigned char i2cSlaveRead7Bits(unsigned long timeout);

unsigned char i2cSlaveReadByte(unsigned long timeout);


unsigned int i2cSlaveRead9Bits(unsigned long timeout);

unsigned char i2cSlaveRead7BitsCopying(unsigned long timeout, unsigned int* sdaPines);

unsigned char i2cSlaveReadByteCopying(unsigned long timeout, unsigned int* sdaPines);

unsigned int i2cSlaveRead9BitsCopying(unsigned long timeout, unsigned int* sdaPines);


/* 
//This Function reads sdaPin (connected to master) and copies it to sdaSlave (connected to Slave)
unsigned char i2cSlaveReadByteCopyingOne(unsigned long timeout, unsigned int sdaSlave);

//This Function reads sdaPin (connected to master) and copies it to all sda ports 
unsigned char i2cSlaveReadByteCopyingAll(unsigned long timeout, unsigned int* sdaPines);
*/
//This function reads a byte from sdaPin (connected to master) and returns it, while a given timeout.
void i2cSlaveWriteByte(unsigned char outByte, unsigned long timeout);

void i2cSlaveWrite9Bits(unsigned int outByte, unsigned long timeout);

// This function reads a byte from sda port(connected to slave) and puts it to sdaPin(connected to master)
unsigned int i2cSlaveWriteByteCopying(unsigned int slaveSda, unsigned long timeout);

// This function reads 9 bits from sda port(connected to slave) and puts it to sdaPin(connected to master)
unsigned int i2cSlaveWrite9BitsCopying(unsigned int slaveSda, unsigned long timeout);

// ******************* ACK AND NACK SLAVE FUNCTIONS  *********************************

// This slave send a NACK to the Master.
void i2cSlaveNack(unsigned long timeout);

// This Slave reads all his sda ports looking for a NACK from other slave. If a NACK is recieved, send a NACK to the master
// and returns the sdaPines' position where the NACK was found
unsigned int i2cSlaveNackFromSlave(unsigned long timeout, unsigned int* sdaPines);

unsigned int i2cSlaveNackFromOneSlave(unsigned long timeout, unsigned int sdaSlave);

// This slave send an ACK to the Master.
void i2cSlaveAck(unsigned long timeout);

// ******************* START AND STOP SLAVE CHECKING FUNCTIOINS  *********************************

//wait for the start condition while a given timeout
bool i2cSlaveWaitStartCondition(unsigned long timeout);
//wait for the start condition, copying the sdaPin(connected to master) to all ports from sdaPines array, while a given timeout
bool i2cSlaveWaitStartConditionCopying(unsigned long timeout, unsigned int* sdaPines);
//wait for the stop condition while a given timeout
bool i2cSlaveWaitStopCondition(unsigned long timeout);
//wait for the stop condition, copying the sdaPin(connected to master) to all ports from sdaPines array, while a given timeout
bool i2cSlaveWaitStopConditionCopying(unsigned long timeout, unsigned int* sdaPines);

// ******************* SLAVE ADDITIONAL PROTOCOL FUNCTION *****************************

//Read only 1 bit from master
bool i2cSlaveReadBit(unsigned long timeout);
//Read 1 bit from sdaPin (connected to Master) and copies this bit to the sdaSlave port (connected to Slave)
bool i2cSlaveReadBitCopying(unsigned long timeout, unsigned int sdaSlave);

// #######################################   END  SLAVE   FUNCTIONS   ###########################################################



// #############################################   MASTER   FUNCTIONS   ##########################################################

// ******************* SDA, SCL MASTER CONTROL ********************************* 
void i2cMasterHighSda(unsigned int wait_us);

void i2cMasterLowSda(unsigned int wait_us);

void i2cMasterHighScl(unsigned int wait_us);

void i2cMasterLowScl(unsigned int wait_us);

// ******************* READ AND WRITE BYTE MASTER FUNCTIOINS  *********************************


void i2cMasterWrite7Bits(unsigned int wait_us, unsigned int out7Bits);


void i2cMasterWriteByte(unsigned int wait_us, unsigned int outByte);

void i2cMasterWrite9Bits(unsigned int wait_us, unsigned int out9Bits);

unsigned char i2cMasterRead7Bits(unsigned int wait_us);

unsigned char i2cMasterReadByte(unsigned int wait_us);

unsigned int i2cMasterRead9Bits(unsigned int wait_us);

 
// ******************* ACK AND NACK SLAVE FUNCTIONS  *********************************

void i2cMasterNack(unsigned int wait_us);

void i2cMasterAck(unsigned int wait_us);

// ******************* START AND STOP MASTER SET FUNCTIOINS  *********************************

void i2cMasterStart(unsigned int wait_us);

void i2cMasterStop(unsigned int wait_us);

// ******************* MASTER ADDITIONAL PROTOCOL FUNCTION *****************************

// The Master reads one single bit.
bool i2cMasterReadBit(unsigned int wait_us);


// #######################################   END  MASTER   FUNCTIONS   ###########################################################

// #############################################   PARITY   FUNCTIONS   ##########################################################

//This function takes 'data' and adds the parity bit in the ('dataBits' + 1) LSB.  
unsigned int setParityBit(unsigned char data, int dataBits);

//This function returns 1 if the data corresponds to the parity bit (MSB).
bool checkParity(unsigned int data);

//This funciton removes the parity bit (MSB)
unsigned char removeParity(unsigned int parityData, unsigned int dataBits);
 
// #######################################   END  PARITY   FUNCTIONS   ###########################################################

// #######################################   SYNCHRONIZATION   FUNCTIONS   ###########################################################

// Master Sync Up
void i2cMasterSyncUp(unsigned int wait_us);

//Slave Sync Up
unsigned int i2cSlaveSyncUp(void);
unsigned int i2cSlaveSyncUpCopying(unsigned int timeout, unsigned int* sdaPines);
// #######################################   SYNCHRONIZATION   FUNCTIONS   ###########################################################

unsigned char i2cGetSdaRef(unsigned int sda, unsigned int* sda_pines);
// #######################################   END  nuevo  ###########################################################
#endif
