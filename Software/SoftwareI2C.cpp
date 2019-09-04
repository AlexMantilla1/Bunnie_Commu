
#include <BitBangI2C.h>
#include <SoftwareI2C.h>

// Global variables

unsigned char deviceSlaveAdd = 0;		
unsigned int wait_us = 10/3; 		// default value for 10 ms period.

unsigned int sdaPIN = 1;		// Pin for sda, default: 1
unsigned int sclPIN = 2;		// Pin for scl, default: 2


// #######################################   SOFTWARE   FUNCTIONS   ###########################################################

void i2cConfig(unsigned int mode, unsigned int sda_Pin, unsigned int scl_Pin, unsigned int param) {

	i2cPinesDefine(sda_Pin, scl_Pin);
	sdaPIN = sda_Pin;
	sclPIN = scl_Pin;

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
unsigned int i2cSlaveListeningCopying(unsigned int timeout, unsigned char* dataStored, unsigned int* sdaPines) {	//try to use a small timeout (how much?....)


	/******* ALGORITHM: ********
	*  1. Check the sda channel waiting a transaction
	*  2. Once the transaction was found, read the address, while is copy the data to his
	*      othres sda ports.
	*  3. Check if the address owns to this slave, if not, check the others sda ports.   
	*  4. Respond to the master, according point 3.
	*  5. Continue the comm copying the data to the sda port as a wire.   
	********************************/		
		
	
	unsigned char slaveAddReaded = 0, dataAdd = 0, dataReaded = 0;
	bool row = 1; //default read
	unsigned int slaveConnected;
	
	if( i2cSlaveWaitStartConditionCopying( timeout, &sdaPines[0] ) ) {
		//Read byte for slave add + operation
		slaveAddReaded = i2cSlaveReadByteCopying( timeout, &sdaPines[0] );
		row = ( slaveAddReaded & 0x01 );
		slaveAddReaded >>= 1;

		//Responds ACK or NACK to the slave add
		if(slaveAddReaded == deviceSlaveAdd) {
			//send others sdas to low
			i2cSlaveSdasLow( &sdaPines[0] );
			
			//respond Nack for correct slave add
			i2cSlaveNack( timeout );
			
			//Read byte for data dir:
			dataAdd = i2cSlaveReadByte( timeout ); 

			//Responds ACK or NACK to the data add
			if(dataAdd < 100) {
				i2cSlaveNack( timeout );
				if( row ) {    //Master wants to read
					i2cSlaveWriteByte( dataStored[dataAdd], timeout );
					if( i2cSlaveReadBit( timeout ) ) { 
						if( !i2cSlaveWaitStopCondition( timeout ) ) return I2C_SLAVE_STOP_ERROR;
						//If you want to do something with the ACK master's respond
						return I2C_SLAVE_DATA_WRITE_ERROR;
					} else {
						if( !i2cSlaveWaitStopCondition( timeout ) ) return I2C_SLAVE_STOP_ERROR;
						//If you want to do something with the NACK master's respond
						return I2C_OK; 
					}
				} else {    //Master wants to write
					dataReaded = i2cSlaveReadByte( timeout );
					dataStored[dataAdd] = dataReaded;

					//Responds ACK or NACK. 
					if( dataReaded == dataStored[dataAdd] ) {
						i2cSlaveNack( timeout );
						if( !i2cSlaveWaitStopCondition( timeout ) ) return I2C_SLAVE_STOP_ERROR;
						return I2C_OK;
					} else { //Ack for error saving data 
						i2cSlaveAck( timeout );
						if( !i2cSlaveWaitStopCondition( timeout ) ) return I2C_SLAVE_STOP_ERROR;
						return I2C_SLAVE_DATA_READ_ERROR;
					}
				}
			} else { // Ack for wrong data add error 
				i2cSlaveAck( timeout );
				if( !i2cSlaveWaitStopCondition( timeout ) ) return I2C_SLAVE_STOP_ERROR;
				return I2C_SLAVE_DATAADD_READ_ERROR;
			}
		} else { //Ack for wrong Add
			//check the others sdas first 
			//wait response from a slave and send Ack/Nack! 
			slaveConnected = i2cSlaveNackFromSlave( timeout, &sdaPines[0] );
			if( slaveConnected != SLAVE_DIDNT_NACK ) {	//A slave responded and a Nack was sent to master for correct slave add
				//Read and save a byte for dataAdd:
				dataAdd = i2cSlaveReadByteCopying( timeout, &sdaPines[0] ); /////TODO: Agregar param de dir a copiar (Para  no copiar a todos)
				slaveConnected = i2cSlaveNackFromSlave( timeout, &sdaPines[0] );////TODO: Agregar param de dir a copiar (Para  no copiar a todos)
				if( slaveConnected != SLAVE_DIDNT_NACK ) {	//A slave responded and a Nack was sent to master for correct data add
					if( row ) {    //Master wants to read
						i2cSlaveWriteByteCopying( sdaPines[slaveConnected], timeout ); //Copy the slave response.
						
						if( i2cSlaveReadBitCopying( timeout, sdaPines[slaveConnected] ) ) { 
							if( !i2cSlaveWaitStopConditionCopying( timeout, &sdaPines[0] ) ) return I2C_SLAVE_STOP_ERROR;
							//If you want to do something with the ACK master's response
							return I2C_SLAVE_DATA_WRITE_ERROR;
						} else {
							if( !i2cSlaveWaitStopConditionCopying( timeout, &sdaPines[0] ) ) return I2C_SLAVE_STOP_ERROR;
							//If you want to do something with the NACK master's response
							return I2C_OK; 
						}
					} else {    //Master wants to write
						dataReaded = i2cSlaveReadByteCopying( timeout, &sdaPines[0] ); 
						// start NACK/ACK respond for a correct data saved to the master  
						if( i2cSlaveNackFromSlave( timeout, &sdaPines[0] ) != SLAVE_DIDNT_NACK ) { //A slave responded, a Nack was sent to master for correct data
							if( !i2cSlaveWaitStopCondition( timeout ) ) return I2C_SLAVE_STOP_ERROR;
							return I2C_OK;
						} else {	//Any slave responded and an Ack was sent to master for incorrect data add
							if( !i2cSlaveWaitStopCondition( timeout ) ) return I2C_SLAVE_STOP_ERROR;
							return I2C_SLAVE_DATA_READ_ERROR;	 
						}
					}
				} else {	//Any slave responded and an Ack was sent to master for incorrect data add
					if( !i2cSlaveWaitStopCondition( timeout ) ) return I2C_SLAVE_STOP_ERROR;
					return I2C_SLAVE_DATAADD_READ_ERROR;	 
				}
			} else {	//Any slave responded and an Ack was sent to master for incorrect slave add
				if( !i2cSlaveWaitStopCondition( timeout ) ) return I2C_SLAVE_STOP_ERROR;
				return I2C_SLAVE_SLAVEADD_READ_ERROR;
			} 
			// At this point is already responded ACK for slave add to the master
		}
	} else {  //Error for No communication found
		return I2C_SLAVE_NO_COMM_ERROR;
	}
	
}

unsigned int i2cSlaveListening(unsigned int timeout, unsigned char* dataStored) {

	unsigned char slaveAddReaded = 0, dataAdd = 0, dataReaded = 0;
	bool row = 1; //default read
	
	if( i2cSlaveWaitStartCondition(timeout) ) {
		//Read byte for slave add + operation
		slaveAddReaded = i2cSlaveReadByte( timeout );
		row = ( slaveAddReaded & 0x01 );
		slaveAddReaded >>= 1;

		//Responds ACK or NACK. 
		if( slaveAddReaded == deviceSlaveAdd ) {
			i2cSlaveNack( timeout );
     
			//Read byte for data dir:
			dataAdd = i2cSlaveReadByte( timeout ); 

			//Responds ACK or NACK.
			if(dataAdd < 100) {
				i2cSlaveNack( timeout );
				if(row) {    //Master wants to read
					i2cSlaveWriteByte( dataStored[dataAdd], timeout );
					if( i2cSlaveReadBit( timeout ) ) { 
						i2cSlaveWaitStopCondition( timeout );
						return I2C_SLAVE_DATA_WRITE_ERROR;
						/*If you want to do something with the ACK master's respond*/
					} else {
						while( !i2cSlaveWaitStopCondition(timeout) );
						return I2C_OK;
						/*If you want to do something with the NACK master's respond*/ 
					}
				} else {    //Master wants to write
					dataReaded = i2cSlaveReadByte(timeout);
					dataStored[dataAdd] = dataReaded;

					//Responds ACK or NACK.
					if( dataReaded == dataStored[dataAdd] ) {
						i2cSlaveNack(timeout);
						while( !i2cSlaveWaitStopCondition(timeout) );
						return I2C_OK;
					} else { 
						i2cSlaveAck(timeout);
						while( !i2cSlaveWaitStopCondition(timeout) );
						return I2C_SLAVE_DATA_READ_ERROR;
					}
				}
			} else { // Ack for wrong data add error 
				i2cSlaveAck(timeout);
				while( !i2cSlaveWaitStopCondition(timeout) );
				return I2C_SLAVE_DATAADD_READ_ERROR;
			}
		} else { // Ack for wrong Add
			i2cSlaveAck(timeout);
			while( !i2cSlaveWaitStopCondition(timeout) );
			return I2C_SLAVE_SLAVEADD_READ_ERROR;
		}
	} else { //Error for No communication found
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



