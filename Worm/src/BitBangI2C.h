

#ifndef BITBANG_I2C
#define BITBANG_I2C

//#define Arduino_LEDS
#define SAMD11

#ifdef Arduino_LEDS
#include <Arduino.h>
#endif
#ifdef SAMD11
#include "ArduinoLike.h"
#endif

#define RISE_OR_FALL_FLANK_OK		0
#define FALL_FLANK_TIMEOUT_ERROR	20
#define RISE_FLANK_TIMEOUT_ERROR	21
#define SLAVE_NACK			22
#define SLAVE_DIDNT_NACK		23


#define COMM_FOUND		50
#define COMM_NOT_FOUND		49
#define END_COMM		48

#define STOP_TIMEOUT    0xFFF

#ifdef Arduino_LEDS
#define WAITING_FLAG	12
#endif
#ifdef SAMD11
#define WAITING_FLAG    10
#endif


void i2cPinesDefine(uint16_t sda_Pin, uint16_t scl_Pin);

// #######################################   SLAVE   FUNCTIONS   ###########################################################

// ******************* SDA, SCL SLAVE CONTROL *********************************
void i2cSlaveHighSda(void);

void i2cSlaveLowSda(void);

void i2cSlaveHighScl(void);

// ************************** Copying data to the othres sda's ports ****************

void copySda(bool d, uint16_t sdaP);

void i2cSlaveSdasLow(uint16_t* sdaPines);

// ************************** Reading data from the others sda's ports **********************

// ******************* SLAVE NEEDS TO READ THE RISE AND FALL SCL/SDA FLANK *********************************

void turnOnFlag(uint8_t pin);
void turnOffFlag(uint8_t pin);
//Wait the rise flank no matter what. >:v.
void waitUntilRiseFlank(uint8_t pin);
//Waits a rise flank in the port PIN, while a given timeout
uint16_t waitRiseFlank(uint8_t PIN, uint32_t timeout);

//Waits a rise flank in the port PIN, while is copying the sdaPin (connected to master) signal to sda port (connected to slave), all this while a given timeout 
uint16_t waitRiseFlankCopyingOne(uint8_t sclPIN, uint32_t timeout, uint16_t sdaToRead, uint16_t sdaToWrite);

//Waits a rise flank in the port PIN, while is copying the sdaPin (connected to master) signal to all ports from sdaPines array ports(connected to slaves), all this while a given timeout
uint16_t waitRiseFlankCopyingAll(uint8_t PIN, uint32_t timeout, uint16_t* sdaPines);

//Waits a fall flank in the port PIN, while a given timeout
uint16_t waitFallFlank(uint8_t PIN, uint32_t timeout);

//Waits a fall flank in the port PIN, while is copying the sdaPin (connected to master) signal to sda port (connected to slave), all this while a given timeout 
uint16_t waitFallFlankCopyingOne(uint8_t sclPIN, uint32_t timeout, uint16_t sdaToRead, uint16_t sdaToWrite);

//This function waits a fall flank, coping the data in sdaPin to the othres sdaPines.
uint16_t waitFallFlankCopyingAll(uint8_t PIN, uint32_t timeout, uint16_t* sdaPines);

// ******************* READ AND WRITE BYTE SLAVE FUNCTIOINS  *********************************

uint8_t i2cSlaveRead7Bits(uint32_t timeout);

uint8_t i2cSlaveReadByte(uint32_t timeout);


uint16_t i2cSlaveRead9Bits(uint32_t timeout);

uint8_t i2cSlaveRead7BitsCopying(uint32_t timeout, uint16_t* sdaPines);

uint8_t i2cSlaveReadByteCopying(uint32_t timeout, uint16_t* sdaPines);

uint16_t i2cSlaveRead9BitsCopying(uint32_t timeout, uint16_t* sdaPines);

/*
//This Function reads sdaPin (connected to master) and copies it to sdaSlave (connected to Slave)
uint8_t i2cSlaveReadByteCopyingOne(uint32_t timeout, uint16_t sdaSlave);

//This Function reads sdaPin (connected to master) and copies it to all sda ports
uint8_t i2cSlaveReadByteCopyingAll(uint32_t timeout, uint16_t* sdaPines);
*/
//This function reads a byte from sdaPin (connected to master) and returns it, while a given timeout.
void i2cSlaveWriteByte(uint8_t outByte, uint32_t timeout);

void i2cSlaveWrite9Bits(uint16_t outByte, uint32_t timeout);

// This function reads a byte from sda port(connected to slave) and puts it to sdaPin(connected to master)
uint16_t i2cSlaveWriteByteCopying(uint16_t slaveSda, uint32_t timeout);

// This function reads 9 bits from sda port(connected to slave) and puts it to sdaPin(connected to master)
uint16_t i2cSlaveWrite9BitsCopying(uint16_t slaveSda, uint32_t timeout);

// ******************* ACK AND NACK SLAVE FUNCTIONS  *********************************

// This slave send a NACK to the Master.
void i2cSlaveNack(uint32_t timeout);

// This Slave reads all his sda ports looking for a NACK from other slave. If a NACK is recieved, send a NACK to the master
// and returns the sdaPines' position where the NACK was found
uint16_t i2cSlaveNackFromSlave(uint32_t timeout, uint16_t* sdaPines);

uint16_t i2cSlaveNackFromOneSlave(uint32_t timeout, uint16_t sdaSlave);

// This slave send an ACK to the Master.
void i2cSlaveAck(uint32_t timeout);

// ******************* START AND STOP SLAVE CHECKING FUNCTIOINS  *********************************

//wait for the start condition while a given timeout
bool i2cSlaveWaitStartCondition(uint32_t timeout);
//wait for the start condition, copying the sdaPin(connected to master) to all ports from sdaPines array, while a given timeout
bool i2cSlaveWaitStartConditionCopying(uint32_t timeout, uint16_t* sdaPines);
//wait for the stop condition while a given timeout
bool i2cSlaveWaitStopCondition(uint32_t timeout);
//wait for the stop condition, copying the sdaPin(connected to master) to all ports from sdaPines array, while a given timeout
bool i2cSlaveWaitStopConditionCopying(uint32_t timeout, uint16_t* sdaPines);

// ******************* SLAVE ADDITIONAL PROTOCOL FUNCTION *****************************

//Read only 1 bit from master
bool i2cSlaveReadBit(uint32_t timeout);
//Read 1 bit from sdaPin (connected to Master) and copies this bit to the sdaSlave port (connected to Slave)
bool i2cSlaveReadBitCopying(uint32_t timeout, uint16_t sdaSlave);

// #######################################   END  SLAVE   FUNCTIONS   ###########################################################

// #############################################   MASTER   FUNCTIONS   ##########################################################

// ******************* SDA, SCL MASTER CONTROL *********************************
void i2cMasterHighSda(uint16_t wait_us);

void i2cMasterLowSda(uint16_t wait_us);

void i2cMasterHighScl(uint16_t wait_us);

void i2cMasterLowScl(uint16_t wait_us);

// ******************* READ AND WRITE BYTE MASTER FUNCTIOINS  *********************************

void i2cMasterWrite7Bits(uint16_t wait_us, uint16_t out7Bits);

void i2cMasterWriteByte(uint16_t wait_us, uint16_t outByte);

void i2cMasterWrite9Bits(uint16_t wait_us, uint16_t out9Bits);

uint8_t i2cMasterRead7Bits(uint16_t wait_us);

uint8_t i2cMasterReadByte(uint16_t wait_us);

uint16_t i2cMasterRead9Bits(uint16_t wait_us);

// ******************* ACK AND NACK SLAVE FUNCTIONS  *********************************

void i2cMasterNack(uint16_t wait_us);

void i2cMasterAck(uint16_t wait_us);

// ******************* START AND STOP MASTER SET FUNCTIOINS  *********************************

void i2cMasterStart(uint16_t wait_us);

void i2cMasterStop(uint16_t wait_us);

// ******************* MASTER ADDITIONAL PROTOCOL FUNCTION *****************************

// The Master reads one single bit.
bool i2cMasterReadBit(uint16_t wait_us);


// #######################################   END  MASTER   FUNCTIONS   ###########################################################

// #############################################   PARITY   FUNCTIONS   ##########################################################

//This function takes 'data' and adds the parity bit in the ('dataBits' + 1) LSB.
uint16_t setParityBit(uint8_t data, uint8_t dataBits);

//This function returns 1 if the data corresponds to the parity bit (MSB).
bool checkParity(uint16_t data);

//This funciton removes the parity bit (MSB)
uint8_t removeParity(uint16_t parityData, uint16_t dataBits);

// #######################################   END  PARITY   FUNCTIONS   ###########################################################

// #######################################   SYNCHRONIZATION   FUNCTIONS   ###########################################################

// Master Sync Up
void i2cMasterSyncUp(uint16_t wait_us);

//Slave Sync Up
uint16_t i2cSlaveSyncUp(uint32_t timeout);
uint16_t i2cSlaveSyncUpCopying(uint32_t timeout, uint16_t* sdaPines);
// #######################################   SYNCHRONIZATION   FUNCTIONS   ###########################################################

uint8_t i2cGetSdaRef(uint16_t sda, uint16_t* sda_pines);
// #######################################   END  nuevo  ###########################################################
#endif
