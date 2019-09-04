

#ifndef SOFTWARE_I2C
#define SOFTWARE_I2C

#include <stdint.h>
#include <Arduino.h>

#define I2C_OK						0
#define I2C_MASTER_DATA_READ_ERROR			1 
#define I2C_MASTER_SLAVEADD_WRITE_ERROR			2
#define I2C_MASTER_DATAADD_WRITE_ERROR			3
#define I2C_MASTER_DATA_WRITE_ERROR			4
#define I2C_SLAVE_SLAVEADD_READ_ERROR			5
#define I2C_SLAVE_DATAADD_READ_ERROR			6
#define I2C_SLAVE_DATA_READ_ERROR			7
#define I2C_SLAVE_DATA_WRITE_ERROR			8
#define I2C_SLAVE_NO_COMM_ERROR				9
#define I2C_SLAVE_STOP_ERROR				10

//OPERATION MODES
#define MASTER_MODE		0
#define SLAVE_MODE		1

// #######################################   SOFTWARE   FUNCTIONS   ###########################################################

void i2cConfig(unsigned int mode, unsigned int sda_Pin, unsigned int scl_Pin, unsigned int param);
//For slave
unsigned int i2cSlaveListeningCopying(unsigned int timeout, unsigned char* dataStored, unsigned int* sdaPines);
// Master Read
unsigned int i2cMasterRead(unsigned char slaveAdress, unsigned char regAdress, unsigned char* readValue );
// Master Write
unsigned int i2cMasterWrite( unsigned char slaveAdress, unsigned char regAdress, unsigned char newRegValue );


// #######################################   END  SOFTWARE   FUNCTIONS   ###########################################################

#endif
