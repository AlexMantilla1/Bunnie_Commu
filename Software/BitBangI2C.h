#ifndef BITBANG_I2C
#define BITBANG_I2C

#include <Arduino.h>


void i2cPinesDefine(unsigned int sda_Pin, unsigned int scl_Pin);

// #######################################   SLAVE   FUNCTIONS   ###########################################################
// ******************* SDA, SCL SLAVE CONTROL ********************************* 
void i2cSlaveHighSda(void);
void i2cSlaveLowSda(void);
void i2cSlaveHighScl(void);

// ******************* SLAVE NEEDS TO READ THE RISE AND FALL SCL/SDA FLANK *********************************
void waitRiseFlank(unsigned char PIN);
void waitFallFlank(unsigned char PIN);

// ******************* READ AND WRITE BYTE SLAVE FUNCTIOINS  *********************************
unsigned char i2cSlaveReadByte(void);
void i2cSlaveWriteByte(unsigned char outByte);

// ******************* ACK AND NACK SLAVE FUNCTIONS  *********************************
void i2cSlaveNack(void);
void i2cSlaveAck(void);

// ******************* START AND STOP SLAVE CHECKING FUNCTIOINS  *********************************
//wait for the start condition while a given timeout
bool i2cSlaveWaitStartCondition(unsigned int timeout);
//wait for the stop condition while a given timeout
bool i2cSlaveWaitStopCondition(unsigned int timeout);

// ******************* SLAVE ADDITIONAL PROTOCOL FUNCTION *****************************
//Read only 1 bit
bool i2cSlaveReadBit();

// #######################################   END  SLAVE   FUNCTIONS   ###########################################################

// #############################################   MASTER   FUNCTIONS   ##########################################################

// ******************* SDA, SCL MASTER CONTROL ********************************* 
void i2cMasterHighSda(unsigned int wait_ms);
void i2cMasterLowSda(unsigned int wait_ms);
void i2cMasterHighScl(unsigned int wait_ms);
void i2cMasterLowScl(unsigned int wait_ms);

// ******************* READ AND WRITE BYTE MASTER FUNCTIOINS  *********************************
unsigned char i2cMasterReadByte(unsigned int wait_ms);
void i2cMasterWriteByte(unsigned int wait_ms, unsigned char outByte);
 
// ******************* ACK AND NACK SLAVE FUNCTIONS  *********************************
void i2cMasterNack(unsigned int wait_ms);
void i2cMasterAck(unsigned int wait_ms);

// ******************* START AND STOP MASTER SET FUNCTIOINS  *********************************
void i2cMasterStart(unsigned int wait_ms);
void i2cMasterStop(unsigned int wait_ms);

// ******************* MASTER ADDITIONAL PROTOCOL FUNCTION *****************************
bool i2cMasterReadBit(unsigned int wait_ms);


// #######################################   END  MASTER   FUNCTIONS   ###########################################################


#endif
