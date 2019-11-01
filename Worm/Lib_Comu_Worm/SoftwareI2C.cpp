#include <BitBangI2C.h>
#include <SoftwareI2C.h>
// Global variables

unsigned char deviceSlaveAdd = 0;		
unsigned int wait_us = 1000/3; 		// default value for 1 ms period.

unsigned int sdaPIN = 1;		// Pin for sda, default: 1
unsigned int sclPIN = 2;		// Pin for scl, default: 2

unsigned char slaveToSearch = 1;

uint8_t sdaToFind[3] = {10,10,10}; //inizialized in 10 to avoid errors.


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

// Slave acting as a wire
unsigned int i2cSlaveListeningCopying(unsigned long timeout, unsigned char* dataStored, unsigned int* sdaPines) {	//try to use a small timeout (how much?....)


	/******* ALGORITHM: ********
	*  1. Check the sda channel waiting a transaction
	*  2. Once the transaction was found, read the address, while is copy the data to his
	*      othres sda ports.
	*  3. Check if the address owns to this slave, if not, check the others sda ports.   
	*  4. Respond to the master, according point 3.
	*  5. Continue the comm copying the data to the sda port as a wire.   
	********************************/		
		
	unsigned char slaveAddReaded = 0, dataAdd = 0;
	unsigned int dataReaded = 0;
	bool row = 1; //default read
	unsigned int slaveConnected;
	unsigned int syncAns = 0;
	syncAns = i2cSlaveSyncUpCopying(timeout,sdaPines);
	if(syncAns == COMM_FOUND) {
		////Serial.println("hola!");
		//for (char i = 0; i < 4; i++) pinMode(sdaPines[i],INPUT);

		if( i2cSlaveWaitStartConditionCopying( timeout, sdaPines ) ) {
			//Read byte for slave add + operation
			slaveAddReaded = i2cSlaveRead7BitsCopying( timeout, sdaPines );
			////Serial.print("slaveAddReaded es: "); //Serial.println(slaveAddReaded,BIN);
			if(checkParity(slaveAddReaded)) {
				row = ( slaveAddReaded & 0x01 );
				slaveAddReaded >>= 1;
				slaveAddReaded = removeParity(slaveAddReaded,6);
				////Serial.print("slaveAddReaded es: "); //Serial.println(slaveAddReaded,BIN);
				//Responds ACK or NACK to the slave add
				
				if(slaveAddReaded == deviceSlaveAdd) {
					for (uint8_t i = 0; i < 4; i++) if (sdaPines[i] != sdaPIN) pinMode(sdaPines[i],INPUT);
					
					//for (char i = 0; i < 4; i++) pinMode(sdaPines[i],INPUT);
					//send others sdas to low
					//i2cSlaveSdasLow( sdaPines ); //This is not necessary
					//respond Nack for correct slave add
					i2cSlaveNack( timeout );
					////Serial.print("deviceSlaveAdd es: "); //Serial.println(deviceSlaveAdd,BIN);
			
					//Read byte for data dir:
					dataAdd = i2cSlaveReadByte( timeout ); 

					if(checkParity(dataAdd)) {
						dataAdd = removeParity(dataAdd,8); //removes the parity bit
						//Responds ACK or NACK to the data add
						if(dataAdd < 128) {
							i2cSlaveNack( timeout );
							if( row ) {    //Master wants to read
								i2cSlaveWrite9Bits( setParityBit(dataStored[dataAdd],8), timeout );
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
								dataReaded = i2cSlaveRead9Bits(timeout);
								if(checkParity(dataReaded)){
									dataReaded = removeParity(dataReaded,9);
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
								} else {
									i2cSlaveAck(timeout);
									while( !i2cSlaveWaitStopCondition(STOP_TIMEOUT) );
									return I2C_SLAVE_DATA_READ_ERROR;
								}
									
							}
						} else { // Ack for wrong data add error 
							i2cSlaveAck( timeout );
							if( !i2cSlaveWaitStopCondition( timeout ) ) return I2C_SLAVE_STOP_ERROR;
							return I2C_SLAVE_DATAADD_READ_ERROR;
						}
					} else { // Ack for wrong data add error 
						i2cSlaveAck(timeout);
						while( !i2cSlaveWaitStopCondition(STOP_TIMEOUT) );
						return I2C_SLAVE_DATAADD_READ_ERROR;
					}
				} else { //Ack for wrong Add
					//check the others sdas first 
					//wait response from a slave and send Ack/Nack!
					////Serial.println("Hola!!!!!!!!!!!!!!!!!");
					slaveConnected = i2cSlaveNackFromSlave( timeout, sdaPines );
					/*
					for (char i = 0; i < 4; i++)
					{
						if (sdaPines[i] != sdaPIN)
						{
							pinMode(sdaPines[i],OUTPUT);
							digitalWrite(sdaPines[i],LOW);
						}
					}
					//Serial.println("END Nack!");
					delay(20000000);
					
					*/
					
					
					////Serial.print("sc"); //Serial.println(slaveConnected,DEC);
					if( slaveConnected != SLAVE_DIDNT_NACK ) {	//A slave responded and a Nack was sent to master for correct slave add
						//Read and save a byte for dataAdd:
						dataAdd = i2cSlaveReadByteCopying( timeout, sdaPines ); 
						if(checkParity(dataAdd)) {
							slaveConnected = i2cSlaveNackFromSlave( timeout, sdaPines );
							if( slaveConnected != SLAVE_DIDNT_NACK ) {	//A slave responded and a Nack was sent to master for correct data add
								if( row ) {    //Master wants to read
									i2cSlaveWrite9BitsCopying( sdaPines[slaveConnected], timeout ); //Copy the slave response.
									if( i2cSlaveReadBitCopying( timeout, sdaPines[slaveConnected] ) ) { 
										if( !i2cSlaveWaitStopConditionCopying( timeout, sdaPines ) ) return I2C_SLAVE_STOP_ERROR;
										//If you want to do something with the ACK master's response
										return I2C_SLAVE_DATA_WRITE_ERROR;
									} else {
										if( !i2cSlaveWaitStopConditionCopying( timeout, sdaPines ) ) return I2C_SLAVE_STOP_ERROR;
										//If you want to do something with the NACK master's response
										return I2C_OK; 
									}
								} else {    //Master wants to write 
									dataReaded = i2cSlaveRead9BitsCopying( timeout, sdaPines );
									/*
									pinMode(sclPIN,OUTPUT);
									digitalWrite(sclPIN,LOW);
									//Serial.print("dataReaded es:"); //Serial.println(dataReaded,BIN);
									//Serial.println("11"); 
									delay(200000000);*/
									if(checkParity(dataReaded)) {
										// start NACK/ACK respond for a correct data saved to the master 
										if( i2cSlaveNackFromSlave( timeout, sdaPines ) != SLAVE_DIDNT_NACK ) { //A slave responded, a Nack was sent to master for correct data
											if( !i2cSlaveWaitStopConditionCopying( timeout, sdaPines ) ) return I2C_SLAVE_STOP_ERROR;
											return I2C_OK;
										} else {	//Any slave responded and an Ack was sent to master for incorrect data add
											if( !i2cSlaveWaitStopConditionCopying( timeout, sdaPines ) ) return I2C_SLAVE_STOP_ERROR;
											return I2C_SLAVE_DATA_READ_ERROR;	 
										}
									} else {
										i2cSlaveAck(timeout);
										while( !i2cSlaveWaitStopConditionCopying( timeout, sdaPines ) );
										return I2C_SLAVE_DATA_READ_ERROR;
									}
								}
							} else {	//Any slave responded and an Ack was sent to master for incorrect data add
								if( !i2cSlaveWaitStopConditionCopying( timeout, sdaPines ) ) {
									return I2C_SLAVE_STOP_ERROR;
								}
								return I2C_SLAVE_DATAADD_READ_ERROR;	 
							}
						} else { // Ack for wrong data add error 
							i2cSlaveAck(timeout);
							while( !i2cSlaveWaitStopConditionCopying( timeout, sdaPines ) );
							return I2C_SLAVE_DATAADD_READ_ERROR;
						}
					} else {	//Any slave responded and an Ack was sent to master for incorrect slave add
						if( !i2cSlaveWaitStopConditionCopying( timeout, sdaPines ) ) {
							////Serial.println("NoS2");
							/*
							for (char i = 0; i < 4; i++)
							{
								pinMode(sdaPines[i],OUTPUT);
								digitalWrite(sdaPines[i],LOW);
							}
							delay(20000000);
							*/
							return I2C_SLAVE_STOP_ERROR;
						}
						return I2C_SLAVE_SLAVEADD_READ_ERROR;
					} 
					// At this point is already responded ACK for slave add to the master
				}
			} else {
				i2cSlaveAck(timeout);
				while( !i2cSlaveWaitStopConditionCopying( timeout, sdaPines ) );
				return I2C_SLAVE_SLAVEADD_READ_ERROR;
			}
			
		} else {  //Error for No communication found
			return I2C_SLAVE_NO_COMM_ERROR;
		}
	} else {
		return syncAns;
	}
}
// Slave Single listening
unsigned int i2cSlaveListening(unsigned long timeout, unsigned char* dataStored, unsigned int* sdaPines) {

	unsigned char slaveAddReaded = 0, dataAdd = 0;
	unsigned int dataReaded = 0;
	bool row = 1; //default read
	
	// Añadir esto
	unsigned char refSda;
	refSda = i2cGetSdaRef( sdaPIN, sdaPines);
	// Hasta aqui
	
	if(i2cSlaveSyncUp(timeout) == COMM_FOUND) {
		/*
		pinMode(sclPIN,OUTPUT);
		digitalWrite(sclPIN,LOW);
		//Serial.print("dataAdd es: "); //Serial.println(dataAdd,BIN);
		Serial.print("sclPIN es: "); Serial.println(sclPIN,DEC);
		Serial.print("sdaPIN es: "); Serial.println(sdaPIN,DEC);
		delay(2000000);
		*/
		//unsigned int error = waitRiseFlank(sclPIN,timeout);
		//pinMode(sdaPines[0],OUTPUT); digitalWrite(sdaPines[0],LOW);
		if( i2cSlaveWaitStartCondition(timeout) /*&& !error*/ ) {
			/*
			pinMode(sclPIN,OUTPUT);
			digitalWrite(sclPIN,LOW);
			//Serial.print("dataAdd es: "); //Serial.println(dataAdd,BIN);
			Serial.print("sclPIN es: "); Serial.println(sclPIN,DEC);
			Serial.print("sdaPIN es: "); Serial.println(sdaPIN,DEC);
			delay(2000000);
			*/
			slaveAddReaded = i2cSlaveRead7Bits( timeout );
			
			if(checkParity(slaveAddReaded)) {
				row = ( slaveAddReaded & 0x01 );
				slaveAddReaded >>= 1;
				slaveAddReaded = removeParity(slaveAddReaded,6);
				//Responds ACK or NACK. 
				Serial.print("slaveAddReaded es: "); Serial.println(slaveAddReaded,BIN);
				////Serial.print("deviceSlaveAdd es: "); //Serial.println(deviceSlaveAdd,BIN);
				if( slaveAddReaded == deviceSlaveAdd ) {
					
					i2cSlaveNack( timeout );
					/*
					pinMode(sclPIN,OUTPUT);
					digitalWrite(sclPIN,LOW);
					//Serial.print("dataAdd es: "); //Serial.println(dataAdd,BIN);
					//Serial.print("sclPIN es: "); //Serial.println(sclPIN,DEC);
					//Serial.print("sdaPIN es: "); //Serial.println(sdaPIN,DEC);
					delay(2000000);
	     			*/
					dataStored[SDA_TO_MASTER] = refSda; // ### Añadir esto !! ###
					
					//Read byte for data dir:
					dataAdd = i2cSlaveReadByte( timeout ); 
					
					
					
					if(checkParity(dataAdd)) {
						dataAdd = removeParity(dataAdd,8); //removes the parity bit
						////Serial.print("dataAdd es: "); //Serial.println(dataAdd,BIN);
						//Responds ACK or NACK.
						if(dataAdd < 128) {
							i2cSlaveNack( timeout );
							if( row ) {    //Master wants to read
								i2cSlaveWrite9Bits( setParityBit(dataStored[dataAdd],8), timeout );
								if( i2cSlaveReadBit( timeout ) ) { 
									i2cSlaveWaitStopCondition( timeout );
									return I2C_SLAVE_DATA_WRITE_ERROR;
									/*If you want to do something with the ACK master's respond*/
								} else {
									/*If you want to do something with the NACK master's respond*/
									dataStored[refSda] = 0x1F; // ### Añadir esto !! ###
									while( !i2cSlaveWaitStopCondition(STOP_TIMEOUT) );
									return I2C_OK;
								}
							} else {    //Master wants to write
								dataReaded = i2cSlaveRead9Bits(timeout);
								////Serial.print("dataReaded es: "); //Serial.println(dataReaded,BIN);
								/*
								pinMode(sclPIN,OUTPUT);
								digitalWrite(sclPIN,LOW);
								//Serial.print("dataReaded es:"); //Serial.println(dataReaded,BIN);
								//Serial.println("11"); 
								delay(200000000);*/
								//aqui va el if si entra le quito la paridad y else
								if(checkParity(dataReaded)) {
									dataReaded = removeParity(dataReaded,9);
									dataStored[dataAdd] = dataReaded;
									
									//Responds ACK or NACK. 
									if( dataReaded == dataStored[dataAdd] ) {
										i2cSlaveNack( timeout );
										/*
										pinMode(sclPIN,OUTPUT);
										digitalWrite(sclPIN,LOW);
										//Serial.print("dataReaded es: "); //Serial.println(dataReaded,BIN);					
										//Serial.println("11");
										delay(2000000);
										*/
										if( !i2cSlaveWaitStopCondition( timeout ) ) return I2C_SLAVE_STOP_ERROR;
										return I2C_OK;
									} else { //Ack for error saving data 
										i2cSlaveAck( timeout );
										/*
										pinMode(sclPIN,OUTPUT);
										digitalWrite(sclPIN,LOW);
										//Serial.print("dataReaded es: "); //Serial.println(dataReaded,BIN);					
										//Serial.println("22");*/
										delay(2000000);
										if( !i2cSlaveWaitStopCondition( timeout ) ) return I2C_SLAVE_STOP_ERROR;
										return I2C_SLAVE_DATA_READ_ERROR;
									}
								}else{
									i2cSlaveAck(timeout);
									while( !i2cSlaveWaitStopCondition(STOP_TIMEOUT) );
									return I2C_SLAVE_DATA_READ_ERROR;
								}
					
							}
						} else { // Ack for wrong data add error 
							i2cSlaveAck(timeout);
							while( !i2cSlaveWaitStopCondition(STOP_TIMEOUT) );
							return I2C_SLAVE_DATAADD_READ_ERROR;
						}
					} else { // Ack for wrong data add error 
						i2cSlaveAck(timeout);
						while( !i2cSlaveWaitStopCondition(STOP_TIMEOUT) );
						return I2C_SLAVE_DATAADD_READ_ERROR;
					}
				} else { // Ack for wrong Add
					i2cSlaveAck(timeout);
					while( !i2cSlaveWaitStopCondition(STOP_TIMEOUT) );
					return I2C_SLAVE_SLAVEADD_READ_ERROR;
				}
			} else {
				i2cSlaveAck(timeout);
				while( !i2cSlaveWaitStopCondition(STOP_TIMEOUT) );
				return I2C_SLAVE_SLAVEADD_READ_ERROR;
			}

		} else { //Error for No communication found
			return I2C_SLAVE_NO_COMM_ERROR;
		}
	} else {
		return COMM_NOT_FOUND;
	}
}

// Master Read
unsigned int i2cMasterRead(unsigned char slaveAdress, unsigned char regAdress, unsigned char* readValue ) {
	unsigned int readedValue = 0; 
	unsigned char slaveAdd;
	
	slaveAdd = setParityBit(((slaveAdress << 1) | 0x1),6);//1 parity bit + 5 SkaveAdd bit + 1 Reading bit.
	i2cMasterSyncUp(wait_us);
	i2cMasterStart(wait_us);
	//direccion del esclavo
	i2cMasterWrite7Bits( wait_us, slaveAdd);// slaveAdd ya tiene 1 bit de paridad+ 5 de direccion +1 de lectura
	//ack
	if( !i2cMasterReadBit(wait_us) ) {			//if slave responds,asdasdasd send the dirrection
		//DIRECCION DATOS
		i2cMasterWriteByte( wait_us, setParityBit(regAdress,7) );
		//ACK
		if(!i2cMasterReadBit(wait_us)) {
			readedValue = i2cMasterRead9Bits(wait_us); 
			
			if( checkParity(readedValue) ) { 
				*readValue = removeParity(readedValue,9);
				i2cMasterNack(wait_us);
				i2cMasterStop(wait_us);
				return I2C_OK;
			}
			else {	//This lines are here as an example if you want to make a confirmation of what you readed. 
				i2cMasterAck(wait_us);
				i2cMasterStop(wait_us);
				return I2C_MASTER_DATA_READ_ERROR;
			}
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
unsigned int i2cMasterWrite( unsigned char slaveAdress, unsigned char regAdress, unsigned char newRegValue ) {
	
	i2cMasterSyncUp(wait_us);
	i2cMasterStart(wait_us);
	i2cMasterWrite7Bits( wait_us, setParityBit((slaveAdress << 1),6));// slaveAdd ya tiene 1 bit de paridad+ 5 de direccion +1 de lectura
	if( !i2cMasterReadBit(wait_us) ) {			//if slave responds, send the dirrection
		i2cMasterWriteByte( wait_us, setParityBit(regAdress,7) );
		if( !i2cMasterReadBit(wait_us) ) {
			i2cMasterWrite9Bits( wait_us, setParityBit(newRegValue,8) );
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

// *********** Useful **************
void i2cSetSlaveAddressesForMaster (unsigned char* slaveAddress, unsigned char* dataStored) {
    for (char i = 0; i < 3; i++) dataStored[MASTER_MEMORY_S1_ADD + i] = slaveAddress[i]<<3;
}
//Set a new sda to communicate
void setSDA (unsigned int newSda) {
	sdaPIN = newSda;
	i2cPinesDefine(newSda, sclPIN);
}
// Get the slave number according his address
unsigned int getSlaveNumber( char add, unsigned char* dataStored ) {
	for (char i = 0; i < 3; i++) if (add == (dataStored[MASTER_MEMORY_S1_ADD + i] >> 3)) return i+1;
}
/*// get the position
unsigned int getSlavePosition( char add, unsigned char* dataStored ) {
	for (char i = 0; i < MASTER_MEMORY_S1_ADD; i++) if(add == dataStored[i]) return i;
}*/
/*
uint8_t getSlaveOffset( uint8_t position, unsigned char* dataStored ) {
	uint8_t i = 4;
	while ( (position - i) > 0 ) i += 4; 
	return i-4;
}*/
//clean the data if an slave has been disconnected.
void cleanSlave( uint8_t slaveNumber, char slaveAdd, unsigned char* dataStored ) {
	for (char i = 0; i < 4; i++) dataStored[4*slaveNumber + i] = 0;
	for (char i = 0; i < MASTER_MEMORY_S1_ADD; i++) if (dataStored[i] == slaveAdd) dataStored[i] = 0;
}

void printMemory(unsigned char* dataStored) {
  //Serial.println("Memory:");
  for( int i = 0; i < (LAST_SDA_TO_SLAVE + 1); i++) {
    //Serial.print(i,DEC); //Serial.print(". "); //Serial.println(dataStored[i],HEX);
  }
}

// ################### MAP FUNCTIONS ###############
// Slave set Up
void i2cSlaveSetUp(unsigned long timeout, unsigned char* dataStored, unsigned int* sdaPines) {
	//guarantee the output is HIGH; 
	//Serial.println("Slave Started!");
	for (char i = 0; i < 4; i++) {
		pinMode(sdaPines[i],INPUT);
		//digitalWrite(sdaPines[i],HIGH);
	}
	unsigned char i=0;
	bool flag = true;
	while(flag) {
		setSDA(sdaPines[i]);
		////Serial.print("port: "); //Serial.println(sdaPines[i],DEC);
		if(i2cSlaveListening( timeout, dataStored, sdaPines ) == I2C_OK) {
			//Serial.println("Se ha encontrado al maestro!");
		} else {
			i++;
			if(i>3) i=0;
		}
		if (dataStored[i] == 0x1F) {
			dataStored[SLAVE_SU_FLAG] = 1;
			flag = false;
		}
		//printMemory(dataStored);
	}
} 



// Master check
unsigned int i2cMasterCheck( unsigned char* dataStored, unsigned int* sdaPines ) {
	
	/******* ALGORITHM: Master map check********
	*  If the device is looking for is not connected (hasn't been detected by the master): 
	*  1. Check all the sda channels (with a reading transaction) waiting for the device.
	*  2. Once the device was found (responds the transaction), reads the address SDA_TO_MASTER.
	*  3. Then writes to the read value before, the value of MASTER_CONNECTED;
	*  4. Stores MASTER_CONNECTED in the "(4*slaveToSearch) + readValue" position in the local memory.
	*  5. Stores the slave add cording the side where the transaction was found.
	*  
	*  If there is a device connected at that side:
	*  1. Reads the LAST_SDA_TO_SLAVE position, of the device conneted at that side's memory. 
	*  2. Check the master local memory at the position of the read value.
	********************************/

	//get the slave Addres to search, according the memory distribution.
	unsigned char slaveAddressToSearch = dataStored[MASTER_MEMORY_S1_ADD + slaveToSearch - 1]; 
	bool slaveConnected = slaveAddressToSearch & 0x1;
	unsigned char readValue = 0;
	bool slaveFound = false;
	unsigned int message = DEVICE_NOT_FOUND;

	slaveAddressToSearch = slaveAddressToSearch >> 3; //Takes only the slave address
	//Serial.print("Buscando al esclavo: "); //Serial.println(slaveAddressToSearch,DEC);
	//Verify if the slave is connected!
	if( slaveConnected ) { // The slave IS connected.
		//Serial.print("En el pin: "); //Serial.println(sdaPines[sdaToFind[slaveToSearch-1]],DEC);
		setSDA(sdaPines[sdaToFind[slaveToSearch-1]]);
		for (char i = 0; i < NUM_OF_TRIES2; i++)	{
			if (i2cMasterWrite(slaveAddressToSearch, SLAVE_MEMORY_STARTUP, 0x1) == I2C_OK)	{ //Slave found! //reading the sda connected to master
				//Serial.println("Se ha completado la coumicacion!");
				slaveFound = true;
				message = DEVICE_STILL_THERE;
				break;
			}
		}
		if (!slaveFound) {
			dataStored[MASTER_MEMORY_S1_ADD + slaveToSearch - 1] = slaveAddressToSearch << 3;
			message = DEVICE_LOST;
		}			
	} else {	// The slave IS NOT connected.
		for (char i = 0; i < 4; i++) { //Checks all the sdas.
		
			//Serial.print("En el pin: "); //Serial.println(sdaPines[i],DEC);
			setSDA(sdaPines[i]);
			for (char j = 0; j < NUM_OF_TRIES; j++)	{ //Default 8 tries to communicate.
				if (i2cMasterRead(slaveAddressToSearch, SLAVE_MEMORY_SDA_MASTER, &readValue) == I2C_OK)	{ //Slave found! //reading the sda connected to master
					dataStored[(4*slaveToSearch) + readValue] = MASTER_CONNECTED; //copies in local memory too
					sdaToFind[slaveToSearch-1] = i;
					//Serial.print("Se ha encontrado al esclavo: "); //Serial.println(slaveAddressToSearch,DEC);
					if( dataStored[i] == 0 ) {
						dataStored[i] = slaveAddressToSearch; //There wasn't a device connected at this side
						dataStored[MASTER_MEMORY_S1_ADD + slaveToSearch - 1] = (slaveAddressToSearch << 3) | 1;
						slaveFound = true;	
						message = NEW_DEVICE_FOUND;
					} else { //There was a device connected at this sidev
						//Serial.println("Ya había uno ahí!");
						char slaveAddress = dataStored[i]; 
						char slaveNumber = getSlaveNumber(slaveAddress,dataStored);
						//Serial.print("slaveAddress es: "); //Serial.println(slaveAddress,BIN);
						//Serial.print("slaveNumber es: "); //Serial.println(slaveNumber,BIN);
						bool lfs = true; //looking for slave position.
						do {
							for (char l = 0 ; l < NUM_OF_TRIES2 ; l++) {
								unsigned int algo = i2cMasterRead(slaveAddress, SLAVE_MEMORY_SDA_SLAVE, &readValue); 
								//Serial.print("algo es: "); //Serial.println(algo,DEC);
								if( algo == I2C_OK ) { //reading the sda connected to slave
									//Serial.print("dataStored[(4*slaveNumber) + readValue] es: "); //Serial.println(dataStored[(4*slaveNumber) + readValue],DEC);
									if( dataStored[(4*slaveNumber) + readValue] == 0 ) {
										lfs = false;
										break;
									} else {
										slaveAddress = dataStored[(4*slaveNumber) + (readValue)];
										slaveNumber = getSlaveNumber(slaveAddress,dataStored);
									}
								} else {
									Serial.println("Error leyendo SLAVE_MEMORY_SDA_SLAVE");
								}
							}
						} while (lfs);
						for (char m = 1 ; m < NUM_OF_TRIES2 ; m++) {
							if( i2cMasterWrite(slaveAddress, readValue, slaveAddressToSearch) == I2C_OK ) { //writing the device connected
								dataStored[(4*slaveNumber) + readValue] = slaveAddressToSearch;
								dataStored[MASTER_MEMORY_S1_ADD + slaveToSearch - 1] = (slaveAddressToSearch << 3) | 1;
								slaveFound = true;	
								message = NEW_DEVICE_FOUND;													
								break;
							} else {
								Serial.println("Error Escribiendo SLAVE_MEMORY_SDA_SLAVE");
							}
						}
					}
				}
				if (slaveFound) break;
			}
			if (slaveFound) break;
		}	
	}

	if (!slaveFound & slaveConnected) {	//The slave was disconnected!
		cleanSlave(slaveToSearch,slaveAddressToSearch,dataStored);
	}
	
	slaveToSearch++;
	if(slaveToSearch > 3) slaveToSearch = 1; 
	
	return message;
}
// ################# END MAP FUNCTIONS #############
