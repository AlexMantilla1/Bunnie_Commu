
#include <BitBangI2C.h>

// Global variables

unsigned int sdaPin = 1;			// Pin for sda, default: 1
unsigned int sclPin = 2;			// Pin for scl, default: 2

void i2cPinesDefine(unsigned int sda_Pin, unsigned int scl_Pin) {
	sdaPin = sda_Pin;
	sclPin = scl_Pin;
}

// #######################################   SLAVE   FUNCTIONS   ###########################################################

// ******************* SDA, SCL SLAVE CONTROL ********************************* 
void i2cSlaveHighSda(void) {
	pinMode(sdaPin,INPUT); 		// bring SDA to high impedance 
}

void i2cSlaveLowSda(void) {		//aqui puede haber un error!
	digitalWrite(sdaPin,LOW); 
	pinMode(sdaPin,OUTPUT);	
}

void i2cSlaveHighScl(void) {
	pinMode(sclPin,INPUT); 		// bring SCL to high impedance 
}

// ************************** Copying data to the othres sda's ports ****************

void copySda(bool d, unsigned int sdaP) {
	pinMode(sdaP,OUTPUT);
	digitalWrite(sdaP,d);
}

void i2cSlaveSdasLow(unsigned int* sdaPines) {
	for( int i = 0 ; i < 4 ; i++ ) if( sdaPines[i] != sdaPin ) {
		pinMode( sdaPines[i], OUTPUT );
		digitalWrite( sdaPines[i], LOW );
	}
}

// ************************** Reading data from the others sda's ports **********************



// ******************* SLAVE NEEDS TO READ THE RISE AND FALL SCL/SDA FLANK *********************************

//Waits a rise flank in the port PIN, while a given timeout
unsigned int waitRiseFlank(unsigned char PIN, unsigned int timeout) {
	while( digitalRead(PIN) ) {
		timeout--;
		if( timeout == 0 ) return RISE_FLANK_TIMEOUT_ERROR; 
	}
	while( !digitalRead(PIN) ) {
		timeout--;
		if( timeout == 0 ) return RISE_FLANK_TIMEOUT_ERROR;
	}
	return RISE_OR_FALL_FLANK_OK; 
}

//Waits a rise flank in the port PIN, while is copying the sdaPin (connected to master) signal to sda port (connected to slave), all this while a given timeout 
unsigned int waitRiseFlankCopying(unsigned char PIN, unsigned int timeout, unsigned int sda) {
	while( digitalRead(PIN) ) {
		copySda( digitalRead(sdaPin), sda );
		timeout--;
		if( timeout == 0 ) return RISE_FLANK_TIMEOUT_ERROR; 
	}
	while( !digitalRead(PIN) ) {
		copySda( digitalRead(sdaPin), sda );
		timeout--;
		if( timeout == 0 ) return RISE_FLANK_TIMEOUT_ERROR;
	}
	return RISE_OR_FALL_FLANK_OK;
}

//Waits a rise flank in the port PIN, while is copying the sdaPin (connected to master) signal to all ports from sdaPines array ports(connected to slaves), all this while a given timeout
unsigned int waitRiseFlankCopyingAll(unsigned char PIN, unsigned int timeout, unsigned int* sdaPines) {
	while( digitalRead(PIN) ) {
		for( int i = 0; i < 4 ; i++ ) if( sdaPines[i] != sdaPin ) copySda( digitalRead(sdaPin), sdaPines[i] );
		timeout--;
		if( timeout == 0 ) return RISE_FLANK_TIMEOUT_ERROR; 
	}
	while( !digitalRead(PIN) ) {
		for( int i = 0; i < 4 ; i++ ) if( sdaPines[i] != sdaPin ) copySda( digitalRead(sdaPin), sdaPines[i] );
		timeout--;
		if( timeout == 0 ) return RISE_FLANK_TIMEOUT_ERROR;
	}
	return RISE_OR_FALL_FLANK_OK;
}

//Waits a fall flank in the port PIN, while a given timeout
unsigned int waitFallFlank(unsigned char PIN, unsigned int timeout) {
	while(!digitalRead(PIN)) { 
		timeout--;
		if( timeout == 0 ) return FALL_FLANK_TIMEOUT_ERROR; 
	}
	while(digitalRead(PIN)) { 
		timeout--;
		if( timeout == 0 ) return FALL_FLANK_TIMEOUT_ERROR; 
	}
	return RISE_OR_FALL_FLANK_OK;
}

//Waits a fall flank in the port PIN, while is copying the sdaPin (connected to master) signal to sda port (connected to slave), all this while a given timeout 
unsigned int waitFallFlankCopying(unsigned char PIN, unsigned int timeout, unsigned int sda) {
	while(!digitalRead(PIN)) {
		copySda( digitalRead(sdaPin), sda );
		timeout--;
		if( timeout == 0 ) return FALL_FLANK_TIMEOUT_ERROR; 
	}
	while(digitalRead(PIN)) {
		copySda( digitalRead(sdaPin), sda );
		timeout--;
		if( timeout == 0 ) return FALL_FLANK_TIMEOUT_ERROR; 
	}
	return RISE_OR_FALL_FLANK_OK;
}

unsigned int waitFallFlankCopyingAll(unsigned char PIN, unsigned int timeout, unsigned int* sdaPines) {
	while(!digitalRead(PIN)) {
		for( int i = 0; i < 4 ; i++) if( sdaPines[i] != sdaPin ) copySda( digitalRead(sdaPin), sdaPines[i] );
		timeout--;
		if( timeout == 0 ) return FALL_FLANK_TIMEOUT_ERROR; 
	}
	while(digitalRead(PIN)) {
		for( int i = 0; i < 4 ; i++) if( sdaPines[i] != sdaPin ) copySda( digitalRead(sdaPin), sdaPines[i] );
		timeout--;
		if( timeout == 0 ) return FALL_FLANK_TIMEOUT_ERROR; 
	}
	return RISE_OR_FALL_FLANK_OK; 
}

// ******************* READ AND WRITE BYTE SLAVE FUNCTIOINS  *********************************

unsigned char i2cSlaveReadByte(unsigned int timeout) {
	unsigned char inByte = 0;
	for (int i = 0; i < 8; ++i) {
		waitRiseFlank( sclPin, timeout );
		if ( digitalRead( sdaPin ) ) {
			inByte = (inByte << 1) | 0x1; // msbit first
		} else {
			inByte = (inByte << 1);
		}
	}
	waitFallFlank( sclPin, timeout );
	return inByte;
}

unsigned char i2cSlaveReadByteCopying(unsigned int timeout, unsigned int* sdaPines) {
	unsigned char inByte = 0;
	for (int i = 0; i < 8; ++i) {
		waitRiseFlankCopyingAll( sclPin, timeout, sdaPines );
		if ( digitalRead( sdaPin ) ) {
			inByte = (inByte << 1) | 0x01; // msbit first
		} else {
			inByte = (inByte << 1);
		}
	}
	waitFallFlankCopyingAll( sclPin, timeout, sdaPines );
	return inByte;
}

//This function reads a byte from sdaPin (connected to master) and returns it, while a given timeout.
void i2cSlaveWriteByte(unsigned char outByte, unsigned int timeout) {
	for (int i = 0; i < 8; ++i) {		
		if( outByte & 0x80 ) i2cSlaveHighSda();  //msbit first
		else i2cSlaveLowSda();
		outByte = outByte << 1;
		waitFallFlank( sclPin, timeout );
	}
	i2cSlaveHighSda();
}

// This function reads a byte from sda port(connected to slave) and puts it to sdaPin(connected to master)
void i2cSlaveWriteByteCopying(unsigned int slaveSda, unsigned int timeout) {
	pinMode(slaveSda,INPUT);
	for (int i = 0; i < 8; ++i) {
		for(int j = 0; j < 0 ; j++){}
		if( digitalRead(slaveSda) ) i2cSlaveHighSda();  //msbit first
		else i2cSlaveLowSda();
		waitFallFlank( sclPin, timeout );
	}
	i2cSlaveHighSda();
}

// ******************* ACK AND NACK SLAVE FUNCTIONS  *********************************

void i2cSlaveNack(unsigned int timeout) {	//Signal For a correct recieved message 
	i2cSlaveLowSda();
	waitFallFlank( sclPin, timeout );
	i2cSlaveHighSda();
}


unsigned int i2cSlaveNackFromSlave(unsigned int timeout, unsigned int* sdaPines) {	//Signal For a correct recieved message

	while( !digitalRead(sclPin) )
		for( int i = 0 ; i < 4 ; i++ ) 
			if( sdaPines[i] != sdaPin ) {
				pinMode(sdaPines[i],INPUT);
				if( !digitalRead( sdaPines[i] ) ) {	//Other slave responded!
					i2cSlaveNack( timeout );
					pinMode(sdaPines[i],OUTPUT);
					return i;
				} else {
					pinMode(sdaPines[i],OUTPUT);
				}
			}
	return SLAVE_DIDNT_NACK;
}


void i2cSlaveAck(unsigned int timeout) {	//Signal For an incorrect message recieved 
	i2cSlaveHighSda();
	waitFallFlank( sclPin, timeout );
	i2cSlaveHighSda();
}

// ******************* START AND STOP SLAVE CHECKING FUNCTIOINS  *********************************

//wait for the start condition while a given timeout
bool i2cSlaveWaitStartCondition(unsigned int timeout) { 
	i2cSlaveHighScl();
	i2cSlaveHighSda();
	if(digitalRead(sclPin) & digitalRead(sdaPin)) {
		if( waitFallFlank(sdaPin, timeout) == RISE_OR_FALL_FLANK_OK )
			if( waitFallFlank(sclPin, timeout) == RISE_OR_FALL_FLANK_OK ) return 1;
			else return 0;
		else return 0; 
		/*
		while(digitalRead(sclPin)) {
			if(!digitalRead(sdaPin)) {
			while(digitalRead(sclPin)) {
				if(timeout == 0) return 0;
				timeout--;
			}
			return 1; 
			}
			if(timeout == 0) return 0;
			timeout--; 
		}
		*/
	} else return 0;
}
//wait for the start condition, copying the sdaPin(connected to master) to all ports from sdaPines array, while a given timeout
bool i2cSlaveWaitStartConditionCopying(unsigned int timeout, unsigned int* sdaPines) { 
	i2cSlaveHighScl();
	i2cSlaveHighSda();
	if(digitalRead(sclPin) & digitalRead(sdaPin)) {
		if( waitFallFlankCopyingAll( sdaPin, timeout, sdaPines ) == RISE_OR_FALL_FLANK_OK ) {
			if( waitFallFlankCopyingAll( sclPin, timeout, sdaPines ) == RISE_OR_FALL_FLANK_OK ) return 1;
			else return 0; 
		} else return 0; 
		
		/*
		while(digitalRead(sclPin)) {
			if(!digitalRead(sdaPin)) {
			while(digitalRead(sclPin)) {
				if(timeout == 0) return 0;
				timeout--;
			}
			return 1; 
			}
			if(timeout == 0) return 0;
			timeout--; 
		}
		*/
	} else return 0;
}
//wait for the stop condition while a given timeout
bool i2cSlaveWaitStopCondition(unsigned int timeout) {
	//i2cSlaveHighScl();
	//i2cSlaveHighSda();
	if(!digitalRead(sclPin) & !digitalRead(sdaPin)) {
		if( waitRiseFlank( sclPin, timeout ) == RISE_OR_FALL_FLANK_OK ) {
			if( waitRiseFlank( sdaPin, timeout ) == RISE_OR_FALL_FLANK_OK ) return 1;
			else return 0; 
		} else return 0; 
		
		/*
		while(digitalRead(sclPin)) {
			if(!digitalRead(sdaPin)) {
			while(digitalRead(sclPin)) {
				if(timeout == 0) return 0;
				timeout--;
			}
			return 1; 
			}
			if(timeout == 0) return 0;
			timeout--; 
		}
		*/
	} else return 0;
}
//wait for the stop condition, copying the sdaPin(connected to master) to all ports from sdaPines array, while a given timeout
bool i2cSlaveWaitStopConditionCopying(unsigned int timeout, unsigned int* sdaPines) {
	//i2cSlaveHighScl();
	//i2cSlaveHighSda();
	if(!digitalRead(sclPin) & !digitalRead(sdaPin)) {
		if( waitRiseFlankCopyingAll( sclPin, timeout, sdaPines ) == RISE_OR_FALL_FLANK_OK ) {
			if( waitRiseFlankCopyingAll( sdaPin, timeout, sdaPines ) == RISE_OR_FALL_FLANK_OK ) return 1;
			else return 0; 
		} else return 0; 
		
		/*
		while(digitalRead(sclPin)) {i2cSlaveNackFromSlave
			if(!digitalRead(sdaPin)) {
			while(digitalRead(sclPin)) {
				if(timeout == 0) return 0;
				timeout--;
			}
			return 1; 
			}
			if(timeout == 0) return 0;
			timeout--; 
		}
		*/
	} else return 0;
}

// ******************* SLAVE ADDITIONAL PROTOCOL FUNCTION *****************************

//Read only 1 bit from master
bool i2cSlaveReadBit(unsigned int timeout) {
	i2cSlaveHighSda();
	waitRiseFlank( sclPin, timeout );
	bool bit = digitalRead(sdaPin);
	waitFallFlank( sclPin, timeout );
	return bit;
}
//Read 1 bit from sdaPin (connected to Master) and copies this bit to the sda port (connected to Slave)
bool i2cSlaveReadBitCopying(unsigned int timeout, unsigned int sda) {
	i2cSlaveHighSda();
	waitRiseFlankCopying( sclPin, timeout, sda );
	bool bit = digitalRead(sdaPin);
	waitFallFlankCopying( sclPin, timeout, sda );
	return bit;
}

// #######################################   END  SLAVE   FUNCTIONS   ###########################################################



// #############################################   MASTER   FUNCTIONS   ##########################################################

// ******************* SDA, SCL MASTER CONTROL ********************************* 
void i2cMasterHighSda(unsigned int wait_us) {
   pinMode(sdaPin, INPUT); //SDA_DIR = 1;   // bring SDA to high impedance
   delayMicroseconds(wait_us); //delayMicroseconds(wait_us);//delayUs(5);
}

void i2cMasterLowSda(unsigned int wait_us) {
   digitalWrite(sdaPin, LOW);
   pinMode(sdaPin, OUTPUT);		// output a logic zero
   delayMicroseconds(wait_us); 				//delayMicroseconds(wait_us);//delayUs(5);
}

void i2cMasterHighScl(unsigned int wait_us) {
   pinMode(sclPin, INPUT);		// bring SCL to high impedance
   delayMicroseconds(wait_us); 				//delayMicroseconds(wait_us);//delayUs(5);
}

void i2cMasterLowScl(unsigned int wait_us) {
   digitalWrite(sclPin, LOW); 	 
   pinMode(sclPin, OUTPUT); 		
   delayMicroseconds(wait_us);				//delayMicroseconds(wait_us);//delayUs(5);
}

// ******************* READ AND WRITE BYTE MASTER FUNCTIOINS  *********************************

unsigned char i2cMasterReadByte(unsigned int wait_us) {
	unsigned char inByte =0 , n;
	i2cMasterHighSda(wait_us);
	for (n=0; n<8; n++) {
		i2cMasterHighScl(wait_us); 
		if (digitalRead(sdaPin)){
			inByte = (inByte << 1) | 0x01; // msbit first
		} else {
			inByte = inByte << 1;
		}
		i2cMasterLowScl(wait_us);
		if(n != 7) {
			delayMicroseconds(wait_us); //delayMicroseconds(wait_us);//delayUs(5);
		}
	}
	return(inByte);
}


void i2cMasterWriteByte(unsigned int wait_us, unsigned char outByte) {
	unsigned char n;
	for(n=0; n<8; n++) {
		if(outByte&0x80) i2cMasterHighSda(wait_us);
		else i2cMasterLowSda(wait_us);
		i2cMasterHighScl(wait_us);
		i2cMasterLowScl(wait_us);
		outByte = outByte << 1;
	}
	i2cMasterHighSda(wait_us);
}
 
// ******************* ACK AND NACK SLAVE FUNCTIONS  *********************************

void i2cMasterNack(unsigned int wait_us) {  
   i2cMasterLowSda(wait_us);
   i2cMasterHighScl(wait_us);
   i2cMasterLowScl(wait_us);    // bring data high and clock
}

void i2cMasterAck(unsigned int wait_us) {
   i2cMasterLowSda(wait_us); 
   i2cMasterHighScl(wait_us);
   i2cMasterLowScl(wait_us);
   i2cMasterHighSda(wait_us);    // bring data low and clock
}

// ******************* START AND STOP MASTER SET FUNCTIOINS  *********************************

void i2cMasterStart(unsigned int wait_us) {
   i2cMasterHighSda(wait_us);
   i2cMasterHighScl(wait_us);  
   i2cMasterLowSda(wait_us);
   i2cMasterLowScl(wait_us);   // bring SDA low while SCL is high
}

void i2cMasterStop(unsigned int wait_us) {
   digitalWrite(sdaPin, LOW);
	pinMode(sdaPin,OUTPUT);
   i2cMasterLowScl(wait_us);
   i2cMasterLowSda(wait_us);
   i2cMasterHighScl(wait_us);
   i2cMasterHighSda(wait_us);    // bring SDA high while SCL is high
}

// ******************* MASTER ADDITIONAL PROTOCOL FUNCTION *****************************

bool i2cMasterReadBit(unsigned int wait_us) { 
	if (digitalRead(sdaPin)) {
		i2cMasterHighScl(wait_us); 
		i2cMasterHighSda(wait_us);
		i2cMasterLowScl(wait_us);
		return 1;
	} else {
		i2cMasterHighScl(wait_us); 
		i2cMasterLowSda(wait_us);
		i2cMasterLowScl(wait_us);
		return 0;
	}
}


// #######################################   END  MASTER   FUNCTIONS   ###########################################################

