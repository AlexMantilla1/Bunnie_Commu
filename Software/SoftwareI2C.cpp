
#include <BitBangI2C.h>
#include <SoftwareI2C.h>

// Global variables

unsigned char deviceSlaveAdd = 0;		
unsigned int wait_us = 10/3; 		// default value for 10 ms period.
unsigned int sdaPIN = 1;		// Pin for sda, default: 1
unsigned int sclPIN = 2;		// Pin for scl, default: 2

// #######################################   SOFTWARE   FUNCTIONS   ###########################################################

void i2cConfig(unsigned int mode, unsigned int sda_Pin, unsigned int scl_Pin, unsigned int param) {
	
	sdaPIN = sda_Pin;
	sclPIN = scl_Pin;

	i2cPinesDefine(sdaPIN, sclPIN);

	if (mode == SLAVE_MODE) {
		//setup slave
		i2cSlaveHighSda();
		i2cSlaveHighScl();
		deviceSlaveAdd = param;
	} else if (mode == MASTER_MODE) { 
		//setup master
		wait_us = param/3;
	}
	
}

//For slave
unsigned int i2cSlaveListening(unsigned int timeout, unsigned char* dataStored) {

	unsigned char slaveAddReaded = 0, dataDir = 0, dataReaded = 0;
	bool row = 1; //default read
	
	if(i2cSlaveWaitStartCondition(timeout)) {
		//Read byte for slave add + operation
		slaveAddReaded = i2cSlaveReadByte();
		row = (slaveAddReaded & 0x01);
		slaveAddReaded >>= 1;

		//Responds ACK or NACK.
		waitFallFlank(sclPIN);
		if(slaveAddReaded == deviceSlaveAdd) {
			i2cSlaveNack();
     
			//Read byte for data dir:
			dataDir = i2cSlaveReadByte(); 

			//Responds ACK or NACK.
			waitFallFlank(sclPIN);
				if(dataDir < 100) {
				i2cSlaveNack();
				if(row) {    //Master wants to read
					i2cSlaveWriteByte(dataStored[dataDir]);
					if(i2cSlaveReadBit()) { 
						i2cSlaveWaitStopCondition(timeout);
						return I2C_SLAVE_DATA_WRITE_ERROR;
						/*If you want to do something with the ACK master's respond*/
					} else {
						while( !i2cSlaveWaitStopCondition(timeout) );
						return I2C_OK;
						/*If you want to do something with the NACK master's respond*/ 
					}
				} else {    //Master wants to write
					dataReaded = i2cSlaveReadByte();
					dataStored[dataDir] = dataReaded;

					//Responds ACK or NACK.
					waitFallFlank(sclPIN);
					if(dataReaded == dataStored[dataDir]) {
						i2cSlaveNack();
						while( !i2cSlaveWaitStopCondition(timeout) );
						return I2C_OK;
					} else { 
						i2cSlaveAck();
						while( !i2cSlaveWaitStopCondition(timeout) );
						return I2C_SLAVE_DATA_READ_ERROR;
					}
				}
			} else {
				i2cSlaveAck();
				while( !i2cSlaveWaitStopCondition(timeout) );
				return I2C_SLAVE_DATAADD_READ_ERROR;
			}
		} else {
			i2cSlaveAck();
			while( !i2cSlaveWaitStopCondition(timeout) );
			return I2C_SLAVE_SLAVEADD_READ_ERROR;
		}
	} else {
		return I2C_SLAVE_NO_COMM_ERROR;
	}
	
}

// Master Read
unsigned int i2cMasterRead(unsigned char slaveAdress, unsigned char regAdress, unsigned char* readValue ){
	unsigned char readedValue = 0;
	i2cMasterStart(wait_us);
	i2cMasterWriteByte( wait_us, (slaveAdress << 1) | 0x1 );
	if( !i2cMasterReadBit(wait_us) ) {			//if slave responds,asdasdasd send the dirrection
		i2cMasterWriteByte( wait_us, regAdress );
		if(!i2cMasterReadBit(wait_us)){
			readedValue = i2cMasterReadByte(wait_us);
			*readValue = readedValue;
			if(*readValue == readedValue) { 
				i2cMasterNack(wait_us);
			}
			else {	//This lines are here as an example if you want to make a confirmation of what you readed. 
				i2cMasterAck(wait_us);
				return I2C_MASTER_DATA_READ_ERROR;
			}
			i2cMasterStop(wait_us);
			return I2C_OK;
		} else{
			i2cMasterStop(wait_us);
			return I2C_MASTER_DATAADD_WRITE_ERROR; 
		}
	} else{
		i2cMasterStop(wait_us);
		return I2C_MASTER_SLAVEADD_WRITE_ERROR;
	}
}

// Master Write
unsigned int i2cMasterWrite( unsigned char slaveAdress, unsigned char regAdress, unsigned char newRegValue ){
	i2cMasterStart(wait_us);
	i2cMasterWriteByte( wait_us, slaveAdress << 1);
	if( !i2cMasterReadBit(wait_us) ) {			//if slave responds, send the dirrection
		i2cMasterWriteByte( wait_us, regAdress );
		if( !i2cMasterReadBit(wait_us) ) {
			i2cMasterWriteByte( wait_us, newRegValue );
			if( !i2cMasterReadBit(wait_us) ) {
				digitalWrite(sdaPIN,LOW);
				i2cMasterStop(wait_us);
				return I2C_OK;
			} else {
				i2cMasterStop(wait_us);
				return I2C_MASTER_DATA_WRITE_ERROR;
			}
		} else {
			i2cMasterStop(wait_us);
			return I2C_MASTER_DATAADD_WRITE_ERROR; 
		}
	} else {
		i2cMasterStop(wait_us);
		return I2C_MASTER_SLAVEADD_WRITE_ERROR;
	}
}


// #######################################   END  SOFTWARE   FUNCTIONS   ###########################################################



