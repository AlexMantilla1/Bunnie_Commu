

#ifndef SOFTWARE_I2C
#define SOFTWARE_I2C

#include <Arduino.h>
#include <stdint.h>
#include <BitBangI2C.h>

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

#define NEW_DEVICE_FOUND		50
#define DEVICE_STILL_THERE		51
#define DEVICE_LOST				52
#define DEVICE_NOT_FOUND		53

//For Memory 

#define MASTER_MEMORY_RIGHT		    0
#define MASTER_MEMORY_UP		    1
#define MASTER_MEMORY_LEFT		    2
#define MASTER_MEMORY_DOWN		    3
#define MASTER_MEMORY_S1_RIGHT		4
#define MASTER_MEMORY_S1_UP		    5
#define MASTER_MEMORY_S1_LEFT		6
#define MASTER_MEMORY_S1_DOWN		7
#define MASTER_MEMORY_S2_RIGHT		8
#define MASTER_MEMORY_S2_UP		    9
#define MASTER_MEMORY_S2_LEFT		10
#define MASTER_MEMORY_S2_DOWN		11
#define MASTER_MEMORY_S3_RIGHT		12
#define MASTER_MEMORY_S3_UP		    13
#define MASTER_MEMORY_S3_LEFT		14
#define MASTER_MEMORY_S3_DOWN		15
#define MASTER_MEMORY_S1_ADD		16
#define MASTER_MEMORY_S2_ADD		17
#define MASTER_MEMORY_S3_ADD		18

#define SLAVE_MEMORY_RIGHT		0
#define SLAVE_MEMORY_UP			1
#define SLAVE_MEMORY_LEFT		2
#define SLAVE_MEMORY_DOWN		3
#define SLAVE_MEMORY_STARTUP		4
#define SLAVE_MEMORY_SDA_MASTER		5
#define SLAVE_MEMORY_SDA_SLAVE		6

#define RIGHT_REF			0
#define UP_REF				1				
#define LEFT_REF			2
#define DOWN_REF			3
#define SLAVE_SU_FLAG		4
#define SDA_TO_MASTER		5
#define LAST_SDA_TO_SLAVE	6

#define NUM_OF_TRIES        8
#define NUM_OF_TRIES2		50
#define MASTER_CONNECTED	0x1F	

//OPERATION MODES
#define MASTER_MODE		0
#define SLAVE_MODE		1

// #######################################   SOFTWARE   FUNCTIONS   ###########################################################
// Node configuration
void i2cConfig(unsigned int mode, unsigned int sda_Pin, unsigned int scl_Pin, unsigned int param);
// Slave acting as a wire
unsigned int i2cSlaveListeningCopying(unsigned long timeout, unsigned char* dataStored, unsigned int* sdaPines);
// Slave Single listening
unsigned int i2cSlaveListening(unsigned long timeout, unsigned char* dataStored, unsigned int* sdaPines);
// Master Read
unsigned int i2cMasterRead(unsigned char slaveAdress, unsigned char regAdress, unsigned char* readValue );
// Master Write
unsigned int i2cMasterWrite( unsigned char slaveAdress, unsigned char regAdress, unsigned char newRegValue );


// #######################################   END  SOFTWARE   FUNCTIONS   ###########################################################

// *********** Useful **************
void i2cSetSlaveAddressesForMaster (unsigned char* slaveAddress, unsigned char* dataStored);
//Set a new sda to communicate
void setSDA (unsigned int newSda);
// Get the slave number according his address
unsigned int getSlaveNumber( char add, unsigned char* dataStored );
/*// get the position and offset.
unsigned int getSlavePosition( char add, unsigned char* dataStored );
uint8_t getSlaveOffset( uint8_t position, unsigned char* dataStored );
*/
//clean the data if an slave has been disconnected.
void cleanSlave( uint8_t slaveNumber, char slaveAdd, unsigned char* dataStored );

// ################### MAP FUNCTIONS ###############
// Slave set Up
void i2cSlaveSetUp(unsigned long timeout, unsigned char* dataStored, unsigned int* sdaPines);
// Master check
unsigned int i2cMasterCheck( unsigned char* dataStored, unsigned int* sdaPines );
// ################# END MAP FUNCTIONS #############

#endif
