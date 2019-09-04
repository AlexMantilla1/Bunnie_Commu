#ifndef BITBANG_I2C
#define BITBANG_I2C

#include <Arduino.h>

#define RISE_OR_FALL_FLANK_OK		0
#define FALL_FLANK_TIMEOUT_ERROR	20
#define RISE_FLANK_TIMEOUT_ERROR	21
#define SLAVE_DIDNT_NACK		22

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
unsigned int waitRiseFlank(unsigned char PIN, unsigned int timeout);
//Waits a rise flank in the port PIN, while is copying the sdaPin (connected to master) signal to sda port (connected to slave), all this while a given timeout 
unsigned int waitRiseFlankCopying(unsigned char PIN, unsigned int timeout, unsigned int sda);
//Waits a rise flank in the port PIN, while is copying the sdaPin (connected to master) signal to all ports from sdaPines array ports(connected to slaves), all this while a given timeout
unsigned int waitRiseFlankCopyingAll(unsigned char PIN, unsigned int timeout, unsigned int* sdaPines);
//Waits a fall flank in the port PIN, while a given timeout
unsigned int waitFallFlank(unsigned char PIN, unsigned int timeout);
//Waits a fall flank in the port PIN, while is copying the sdaPin (connected to master) signal to sda port (connected to slave), all this while a given timeout 
unsigned int waitFallFlankCopying(unsigned char PIN, unsigned int timeout, unsigned int sda);
unsigned int waitFallFlankCopyingAll(unsigned char PIN, unsigned int timeout, unsigned int* sdaPines);
// ******************* READ AND WRITE BYTE SLAVE FUNCTIOINS  *********************************

unsigned char i2cSlaveReadByte(unsigned int timeout);

unsigned char i2cSlaveReadByteCopying(unsigned int timeout, unsigned int* sdaPines);

//This function reads a byte from sdaPin (connected to master) and returns it, while a given timeout.
void i2cSlaveWriteByte(unsigned char outByte, unsigned int timeout);
// This function reads a byte from sda port(connected to slave) and puts it to sdaPin(connected to master)
void i2cSlaveWriteByteCopying(unsigned int sda, unsigned int timeout);
// ******************* ACK AND NACK SLAVE FUNCTIONS  *********************************

void i2cSlaveNack(unsigned int timeout) ;
unsigned int i2cSlaveNackFromSlave(unsigned int timeout, unsigned int* sdaPines);

void i2cSlaveAck(unsigned int timeout) ;

// ******************* START AND STOP SLAVE CHECKING FUNCTIOINS  *********************************

//wait for the start condition while a given timeout
bool i2cSlaveWaitStartCondition(unsigned int timeout);
//wait for the start condition, copying the sdaPin(connected to master) to all ports from sdaPines array, while a given timeout
bool i2cSlaveWaitStartConditionCopying(unsigned int timeout, unsigned int* sdaPines);
//wait for the stop condition while a given timeout
bool i2cSlaveWaitStopCondition(unsigned int timeout);
//wait for the stop condition, copying the sdaPin(connected to master) to all ports from sdaPines array, while a given timeout
bool i2cSlaveWaitStopConditionCopying(unsigned int timeout, unsigned int* sdaPines);
// ******************* SLAVE ADDITIONAL PROTOCOL FUNCTION *****************************

//Read only 1 bit from master
bool i2cSlaveReadBit(unsigned int timeout);
//Read 1 bit from sdaPin (connected to Master) and copies this bit to the sda port (connected to Slave)
bool i2cSlaveReadBitCopying(unsigned int timeout, unsigned int sda);
// #######################################   END  SLAVE   FUNCTIONS   ###########################################################



// #############################################   MASTER   FUNCTIONS   ##########################################################

// ******************* SDA, SCL MASTER CONTROL ********************************* 
void i2cMasterHighSda(unsigned int wait_us);
void i2cMasterLowSda(unsigned int wait_us);

void i2cMasterHighScl(unsigned int wait_us);

void i2cMasterLowScl(unsigned int wait_us);

// ******************* READ AND WRITE BYTE MASTER FUNCTIOINS  *********************************

unsigned char i2cMasterReadByte(unsigned int wait_us);

void i2cMasterWriteByte(unsigned int wait_us, unsigned char outByte);
 
// ******************* ACK AND NACK SLAVE FUNCTIONS  *********************************

void i2cMasterNack(unsigned int wait_us);
void i2cMasterAck(unsigned int wait_us);

// ******************* START AND STOP MASTER SET FUNCTIOINS  *********************************

void i2cMasterStart(unsigned int wait_us);
void i2cMasterStop(unsigned int wait_us);

// ******************* MASTER ADDITIONAL PROTOCOL FUNCTION *****************************

bool i2cMasterReadBit(unsigned int wait_us);

// #######################################   END  MASTER   FUNCTIONS   ###########################################################

#endif
