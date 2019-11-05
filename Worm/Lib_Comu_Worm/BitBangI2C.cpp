
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
//#define WAITING_FLAG	12
void turnOnFlag(uint8_t pin) {
	pinMode(pin, OUTPUT);
	digitalWrite(pin, HIGH);
}
void turnOffFlag(uint8_t pin) {
	pinMode(pin, OUTPUT);
	digitalWrite(pin,LOW);
}
//Wait the rise flank no matter what. >:v.
void waitUntilRiseFlank(uint8_t pin) {
	turnOnFlag(WAITING_FLAG);
	pinMode(pin,INPUT);
	while ( digitalRead(pin) );
	while ( !digitalRead(pin) );
	turnOffFlag(WAITING_FLAG);
}
//Waits a rise flank in the port PIN, while a given timeout
unsigned int waitRiseFlank(unsigned char PIN, unsigned long timeout) {
	turnOnFlag(WAITING_FLAG);
	while( digitalRead(PIN) ) {
		timeout--;
		if( timeout == 0 ) turnOffFlag(WAITING_FLAG);
		if( timeout == 0 ) return RISE_FLANK_TIMEOUT_ERROR; 
	}
	while( !digitalRead(PIN) ) {
		timeout--;
		if( timeout == 0 ) turnOffFlag(WAITING_FLAG);
		if( timeout == 0 ) return RISE_FLANK_TIMEOUT_ERROR;
	}
	turnOffFlag(WAITING_FLAG);
	return RISE_OR_FALL_FLANK_OK; 
}

//Waits a rise flank in the port PIN, while is copying the sdaPin (connected to master) signal to sda port (connected to slave), all this while a given timeout 
unsigned int waitRiseFlankCopyingOne(unsigned char sclPIN, unsigned long timeout, unsigned int sdaToRead, unsigned int sdaToWrite) {
	turnOnFlag(WAITING_FLAG);
	while( digitalRead(sclPIN) ) {
		copySda( digitalRead(sdaToRead), sdaToWrite );
		timeout--;
		if( timeout == 0 ) turnOffFlag(WAITING_FLAG);
		if( timeout == 0 ) return RISE_FLANK_TIMEOUT_ERROR; 
	}
	while( !digitalRead(sclPIN) ) {
		copySda( digitalRead(sdaToRead), sdaToWrite );
		timeout--;
		if( timeout == 0 ) turnOffFlag(WAITING_FLAG);
		if( timeout == 0 ) return RISE_FLANK_TIMEOUT_ERROR;
	}
	turnOffFlag(WAITING_FLAG);
	return RISE_OR_FALL_FLANK_OK;
}

//Waits a rise flank in the port PIN, while is copying the sdaPin (connected to master) signal to all ports from sdaPines array ports(connected to slaves), all this while a given timeout
unsigned int waitRiseFlankCopyingAll(unsigned char PIN, unsigned long timeout, unsigned int* sdaPines) {
	bool data;
	turnOnFlag(WAITING_FLAG);
	while( digitalRead(PIN) ) {
		data = digitalRead(sdaPin);
		for( int i = 0; i < 4 ; i++ ) if( sdaPines[i] != sdaPin ) copySda( data, sdaPines[i] );
		timeout--;
		if( timeout == 0 ) turnOffFlag(WAITING_FLAG);
		if( timeout == 0 ) return RISE_FLANK_TIMEOUT_ERROR; 
	}
	while( !digitalRead(PIN) ) {
		data = digitalRead(sdaPin);
		for( int i = 0; i < 4 ; i++ ) if( sdaPines[i] != sdaPin ) copySda( data, sdaPines[i] );
		timeout--;
		if( timeout == 0 ) turnOffFlag(WAITING_FLAG);
		if( timeout == 0 ) return RISE_FLANK_TIMEOUT_ERROR;
	}
	data = digitalRead(sdaPin);
	for( int i = 0; i < 4 ; i++ ) if( sdaPines[i] != sdaPin ) copySda( data, sdaPines[i] );
	turnOffFlag(WAITING_FLAG);
	return RISE_OR_FALL_FLANK_OK;
}

//Waits a fall flank in the port PIN, while a given timeout
unsigned int waitFallFlank(unsigned char PIN, unsigned long timeout) {
	turnOnFlag(WAITING_FLAG);
	while(!digitalRead(PIN)) { 
		timeout--;
		if( timeout == 0 ) turnOffFlag(WAITING_FLAG);
		if( timeout == 0 ) return FALL_FLANK_TIMEOUT_ERROR; 
	}
	while(digitalRead(PIN)) { 
		timeout--;
		if( timeout == 0 ) turnOffFlag(WAITING_FLAG);
		if( timeout == 0 ) return FALL_FLANK_TIMEOUT_ERROR; 
	}
	turnOffFlag(WAITING_FLAG);
	return RISE_OR_FALL_FLANK_OK;
}

//Waits a fall flank in the port PIN, while is copying the sdaPin (connected to master) signal to sda port (connected to slave), all this while a given timeout 
unsigned int waitFallFlankCopyingOne(unsigned char sclPIN, unsigned long timeout, unsigned int sdaToRead, unsigned int sdaToWrite) {
	turnOnFlag(WAITING_FLAG);
	while( !digitalRead(sclPIN) ) {
		copySda( digitalRead(sdaToRead), sdaToWrite );
		timeout--;
		if( timeout == 0 ) turnOffFlag(WAITING_FLAG);
		if( timeout == 0 ) return FALL_FLANK_TIMEOUT_ERROR; 
	}
	while( digitalRead(sclPIN) ) {
		copySda( digitalRead(sdaToRead), sdaToWrite );
		timeout--;
		if( timeout == 0 ) turnOffFlag(WAITING_FLAG);
		if( timeout == 0 ) return FALL_FLANK_TIMEOUT_ERROR; 
	}
	turnOffFlag(WAITING_FLAG);
	return RISE_OR_FALL_FLANK_OK;
}

//This function waits a fall flank, coping the data in sdaPin to the othres sdaPines.
unsigned int waitFallFlankCopyingAll(unsigned char PIN, unsigned long timeout, unsigned int* sdaPines) {
	turnOnFlag(WAITING_FLAG);
	while( !digitalRead(PIN) ) {
		for( int i = 0; i < 4 ; i++) if( sdaPines[i] != sdaPin ) copySda( digitalRead(sdaPin), sdaPines[i] );
		timeout--;
		if( timeout == 0 ) turnOffFlag(WAITING_FLAG);
		if( timeout == 0 ) return FALL_FLANK_TIMEOUT_ERROR; 
	}
	while( digitalRead(PIN) ) {
		for( int i = 0; i < 4 ; i++) if( sdaPines[i] != sdaPin ) copySda( digitalRead(sdaPin), sdaPines[i] );
		timeout--;
		if( timeout == 0 ) turnOffFlag(WAITING_FLAG);
		if( timeout == 0 ) return FALL_FLANK_TIMEOUT_ERROR; 
	}
	turnOffFlag(WAITING_FLAG);
	return RISE_OR_FALL_FLANK_OK; 
}

// ******************* READ AND WRITE BYTE SLAVE FUNCTIOINS  *********************************
unsigned char i2cSlaveRead7Bits(unsigned long timeout) {
	unsigned char inByte = 0;
	for (int i = 0; i < 7; ++i) {
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

unsigned char i2cSlaveReadByte(unsigned long timeout) {
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

unsigned int i2cSlaveRead9Bits(unsigned long timeout) {
	unsigned int inByte = 0;
	for (int i = 0; i < 9; ++i) {
		
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

unsigned char i2cSlaveRead7BitsCopying(unsigned long timeout, unsigned int* sdaPines) {
	unsigned char inByte = 0;
	for (int i = 0; i < 7; ++i) {
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

unsigned char i2cSlaveReadByteCopying(unsigned long timeout, unsigned int* sdaPines) {
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

unsigned int i2cSlaveRead9BitsCopying(unsigned long timeout, unsigned int* sdaPines) {
	unsigned int inByte = 0;
	for (int i = 0; i < 9; ++i) {
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
void i2cSlaveWriteByte(unsigned char outByte, unsigned long timeout) {
	for (int i = 0; i < 8; ++i) {		
		if( outByte & 0x80 ) i2cSlaveHighSda();  //msbit first
		else i2cSlaveLowSda();
		outByte = outByte << 1;
		waitFallFlank( sclPin, timeout );
	}
	i2cSlaveHighSda();
}

void i2cSlaveWrite9Bits(unsigned int outByte, unsigned long timeout) {
	for (int i = 0; i < 9; ++i) {		
		if( outByte & 0x100 ) i2cSlaveHighSda();  //msbit first
		else i2cSlaveLowSda();
		outByte = outByte << 1;
		waitFallFlank( sclPin, timeout );
	}
	i2cSlaveHighSda();
}

// This function reads a byte from sda port(connected to slave) and puts it to sdaPin(connected to master)
unsigned int i2cSlaveWriteByteCopying(unsigned int slaveSda, unsigned long timeout) {
	pinMode(slaveSda,INPUT);
	for (int i = 0; i < 8; ++i) {
		while (!digitalRead(sclPin)){ // SCL = LOW
			if( digitalRead(slaveSda) ) i2cSlaveHighSda();  //msbit first
			else i2cSlaveLowSda();
			timeout--;
			if( timeout == 0 ) return RISE_FLANK_TIMEOUT_ERROR;
		}
		while (digitalRead(sclPin)) { // SCL = HIGH
			if( digitalRead(slaveSda) ) i2cSlaveHighSda();  //msbit first
			else i2cSlaveLowSda();
			timeout--;
			if( timeout == 0 ) return FALL_FLANK_TIMEOUT_ERROR;
		}
	}
	i2cSlaveHighSda();
}

// This function reads 9 bits from sda port(connected to slave) and puts it to sdaPin(connected to master)
unsigned int i2cSlaveWrite9BitsCopying(unsigned int slaveSda, unsigned long timeout) {
	pinMode(slaveSda,INPUT);
	for (int i = 0; i < 9; ++i) {
		while (!digitalRead(sclPin)){ //scl en ba
			if( digitalRead(slaveSda) ) i2cSlaveHighSda();  //msbit first
			else i2cSlaveLowSda();
			timeout--;
			if( timeout == 0 ) return FALL_FLANK_TIMEOUT_ERROR;
		}
		while (digitalRead(sclPin)) { 
			if( digitalRead(slaveSda) ) i2cSlaveHighSda();  //msbit first
			else i2cSlaveLowSda();
			timeout--;
			if( timeout == 0 ) return FALL_FLANK_TIMEOUT_ERROR;
		}
	}
	i2cSlaveHighSda();
}

// ******************* ACK AND NACK SLAVE FUNCTIONS  *********************************

// This slave send a NACK to the Master.
void i2cSlaveNack(unsigned long timeout) {	//Signal For a correct recieved message 
	i2cSlaveLowSda();
	waitFallFlank( sclPin, timeout );
	i2cSlaveHighSda();
}

// This Slave reads all his sda ports looking for a NACK from other slave. If a NACK is recieved, send a NACK to the master
// and returns the sdaPines' position where the NACK was found
unsigned int i2cSlaveNackFromSlave(unsigned long timeout, unsigned int* sdaPines) {	//Signal For a correct recieved message
	//This function starts after a scl fall flank!
	for (char i = 0; i < 4; i++) if(sdaPines[i] != sdaPin) pinMode(sdaPines[i],INPUT);
	while( !digitalRead(sclPin) ) // While scl is LOW, looking for some answer.
		for( int i = 0 ; i < 4 ; i++ ) 
			if( sdaPines[i] != sdaPin ) {
				if( !digitalRead( sdaPines[i] ) ) {	//Other slave responded!
					i2cSlaveNack( timeout );
					return i;
				}
			}
	return SLAVE_DIDNT_NACK;
}

unsigned int i2cSlaveNackFromOneSlave(unsigned long timeout, unsigned int sdaSlave) {	//Signal For a correct recieved message
	//This function starts after a scl fall flank!
	while( !digitalRead(sclPin) ) { // While scl is LOW, looking for some answer.
		pinMode(sdaSlave,INPUT);
		if( !digitalRead( sdaSlave ) ) {	//Other slave responded!
			i2cSlaveNack( timeout );
			return sdaSlave;
		}
	}
	return SLAVE_DIDNT_NACK;
}

// This slave send an ACK to the Master.
void i2cSlaveAck(unsigned long timeout) {	//Signal For an incorrect message recieved 
	i2cSlaveHighSda();
	waitFallFlank( sclPin, timeout );
	i2cSlaveHighSda();
}

// ******************* START AND STOP SLAVE CHECKING FUNCTIOINS  *********************************

//wait for the start condition while a given timeout
bool i2cSlaveWaitStartCondition(unsigned long timeout) { 
	i2cSlaveHighScl();
	i2cSlaveHighSda();
	if( waitFallFlank(sdaPin, timeout) == RISE_OR_FALL_FLANK_OK ) {
		if( waitFallFlank(sclPin, timeout) == RISE_OR_FALL_FLANK_OK ) return 1;
		else return 0;
	} else return 0;
}
//wait for the start condition, copying the sdaPin(connected to master) to all ports from sdaPines array, while a given timeout
bool i2cSlaveWaitStartConditionCopying(unsigned long timeout, unsigned int* sdaPines) { 
	i2cSlaveHighScl();
	i2cSlaveHighSda();
	if(digitalRead(sclPin) & digitalRead(sdaPin)) {
		if( waitFallFlankCopyingAll( sdaPin, timeout, sdaPines ) == RISE_OR_FALL_FLANK_OK ) {
			if( waitFallFlankCopyingAll( sclPin, timeout, sdaPines ) == RISE_OR_FALL_FLANK_OK ) return 1;
			else return 0; 
		} else return 0; 
	} else return 0;

}
//wait for the stop condition while a given timeout
bool i2cSlaveWaitStopCondition(unsigned long timeout) {
	if( waitRiseFlank( sclPin, timeout ) == RISE_OR_FALL_FLANK_OK ) {
		if( waitRiseFlank( sdaPin, timeout ) == RISE_OR_FALL_FLANK_OK ) return 1;
		else return 0; 
	} else return 0; 
}
//wait for the stop condition, copying the sdaPin(connected to master) to all ports from sdaPines array, while a given timeout
bool i2cSlaveWaitStopConditionCopying(unsigned long timeout, unsigned int* sdaPines) {
	if( waitRiseFlankCopyingAll( sclPin, timeout, sdaPines ) == RISE_OR_FALL_FLANK_OK ) {
		if( waitRiseFlankCopyingAll( sdaPin, timeout, sdaPines ) == RISE_OR_FALL_FLANK_OK ) return 1;
		else return 0; 
	} else return 0;	
}

// ******************* SLAVE ADDITIONAL PROTOCOL FUNCTION *****************************

//Read only 1 bit from master
bool i2cSlaveReadBit(unsigned long timeout) {
	i2cSlaveHighSda();
	waitRiseFlank( sclPin, timeout );
	bool bit = digitalRead(sdaPin);
	waitFallFlank( sclPin, timeout );
	return bit;
}

//Read 1 bit from sdaPin (connected to Master) and copies this bit to the sdaSlave port (connected to Slave)
bool i2cSlaveReadBitCopying(unsigned long timeout, unsigned int sdaSlave) {
	i2cSlaveHighSda();
	waitRiseFlankCopyingOne( sclPin, timeout, sdaPin, sdaSlave );
	bool bit = digitalRead(sdaPin);
	waitFallFlankCopyingOne( sclPin, timeout, sdaPin, sdaSlave );
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


void i2cMasterWrite7Bits(unsigned int wait_us, unsigned int out7Bits) {
	unsigned char n;
	for(n=0; n<7; n++) {
		if(out7Bits&0x40) i2cMasterHighSda(wait_us);
		else i2cMasterLowSda(wait_us);
		i2cMasterHighScl(wait_us);
		i2cMasterLowScl(wait_us);
		out7Bits = out7Bits << 1;
	}
	i2cMasterHighSda(wait_us);
}

void i2cMasterWriteByte(unsigned int wait_us, unsigned int outByte) {
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

void i2cMasterWrite9Bits(unsigned int wait_us, unsigned int out9Bits) {
	unsigned char n;
	for(n=0; n<9; n++) {
		if(out9Bits&0x100) i2cMasterHighSda(wait_us);
		else i2cMasterLowSda(wait_us);
		i2cMasterHighScl(wait_us);
		i2cMasterLowScl(wait_us);
		out9Bits = out9Bits << 1;
	}
	i2cMasterHighSda(wait_us);
}

unsigned char i2cMasterRead7Bits(unsigned int wait_us) {
	unsigned char inByte =0 , n;
	i2cMasterHighSda(wait_us);
	for (n=0; n<7; n++) { 
		i2cMasterHighScl(wait_us); 
		if (digitalRead(sdaPin)) {
			inByte = (inByte << 1) | 0x01; // msbit first
		} else {
			inByte = inByte << 1;
		}
		i2cMasterLowScl(wait_us);
		if(n != 6) {
			delayMicroseconds(wait_us); //delayMicroseconds(wait_us);//delayUs(5);
		}
	}
	return(inByte);
}

unsigned char i2cMasterReadByte(unsigned int wait_us) {
	unsigned char inByte =0 , n;
	i2cMasterHighSda(wait_us);
	for (n=0; n<8; n++) {
		i2cMasterHighScl(wait_us); 
		if (digitalRead(sdaPin)) {
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

unsigned int i2cMasterRead9Bits(unsigned int wait_us) {
	unsigned int inByte =0 , n;
	i2cMasterHighSda(wait_us);
	for (n=0; n<9; n++) {
		i2cMasterHighScl(wait_us); 
		if (digitalRead(sdaPin)) {
			inByte = (inByte << 1) | 0x01; // msbit first
		} else {
			inByte = inByte << 1;
		}
		i2cMasterLowScl(wait_us);
		if(n != 8) { 
			delayMicroseconds(wait_us); //delayMicroseconds(wait_us);//delayUs(5);
		}
	}
	return(inByte);
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

// The Master reads one single bit.
bool i2cMasterReadBit(unsigned int wait_us) { 
	pinMode(sdaPin,INPUT);
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

// #############################################   PARITY   FUNCTIONS   ##########################################################

//This function takes 'data' and adds the parity bit in the ('dataBits' + 1) LSB.  
unsigned int setParityBit(unsigned char data, int dataBits) {
	unsigned char data1;
	unsigned int count = 0; 
	data1 = data;
	while (data1) { 
	    	count += data1 & 1; 
	    	data1 >>= 1; 
	} 
	count = count & 1 ? 0 : 1;
	if(count) { 
		return data ;
	} else { 
		return (data | (0x1 << dataBits));
	}
}

//This function returns 1 if the data corresponds to the parity bit (MSB).
bool checkParity(unsigned int data) {
	unsigned int count = 0; 
	while (data) { 
		count += data & 1; 
		data >>= 1; 
	}
	count = (count & 1 ? 0 : 1);
	return count;
}

//This funciton removes the parity bit (MSB)
unsigned char removeParity(unsigned int parityData, unsigned int dataBits) {
	return (parityData & ~(0x1 << (dataBits-1)));
}
 
// #######################################   END  PARITY   FUNCTIONS   ###########################################################

// #######################################   SYNCHRONIZATION   FUNCTIONS   ###########################################################

// Master Sync Up
void i2cMasterSyncUp(unsigned int wait_us) {
	pinMode(sdaPin,OUTPUT);
	digitalWrite(sdaPin,LOW);
	delayMicroseconds(wait_us);
	pinMode(sclPin,OUTPUT);
	digitalWrite(sclPin,LOW);
	delay(300);
	pinMode(sdaPin,INPUT);
	delayMicroseconds(wait_us);
	pinMode(sclPin,INPUT);
}

//Slave Sync Up
unsigned int i2cSlaveSyncUp(unsigned long timeout) {
	pinMode(sclPin,INPUT);
	unsigned long i = timeout << 1;
	while(i > 0) {
		if(!digitalRead(sclPin)) {
			if(!digitalRead(sdaPin)) i--;
			else return COMM_NOT_FOUND; //The communication was not with him.
		} else return COMM_NOT_FOUND; //The communication was not with him.
	}
	//Serial.println("Waiting no matter what!"); //eliminar esto.
	while (!digitalRead(sdaPin)); // sda is low and wait for sda Rise flank.
	if(!digitalRead(sclPin)) { //means the flag is for a new communication.
		waitRiseFlank(sclPin,timeout);
		return COMM_FOUND; //The communication was with him.
	} else return END_COMM; //means the flag is for end communication.
	
}
//Slave Sync Up copying
unsigned int i2cSlaveSyncUpCopying(unsigned long timeout, unsigned int* sdaPines) {

	pinMode(sclPin,INPUT);
	bool flag = true; // flag to avoid to be setting 0 in SDAs every cicle.
	unsigned long i = timeout << 1;
	while(i > 0) {
		if(!digitalRead(sclPin)) {
			if(!digitalRead(sdaPin)) {
				i--;
				if (flag) {
					for (char i = 0; i < 4; i++) { //Copies the flag to the othres ports
						if (sdaPines[i] != sdaPin) {
							pinMode(sdaPines[i],OUTPUT);
							digitalWrite(sdaPines[i],LOW);
						}
					}
					flag = false;
				}
			} else {
				//Copies the flag to the othres ports
				for (char i = 0; i < 4; i++) if (sdaPines[i] != sdaPin) pinMode(sdaPines[i],INPUT);			
				return COMM_NOT_FOUND; //The communication was not with him or someone at this side.
			}
		} else {
			//Copies the flag to the othres ports
			for (char i = 0; i < 4; i++) if (sdaPines[i] != sdaPin) pinMode(sdaPines[i],INPUT);			
			return COMM_NOT_FOUND; //The communication was not with him.
		}
	}
	//Serial.println("Waiting no matter what!"); //eliminar esto.
	while (!digitalRead(sdaPin)); // sda is low and wait for sda Rise flank.
	for (char i = 0; i < 4; i++) if (sdaPines[i] != sdaPin) pinMode(sdaPines[i],INPUT);	
	if(!digitalRead(sclPin)) { //means the flag is for a new communication.
		waitRiseFlank(sclPin,timeout);
		return COMM_FOUND; //The communication was with him.
	} else return END_COMM; //means the flag is for end communication.

}
// #######################################  END SYNCHRONIZATION   FUNCTIONS   ###########################################################


// #######################################   nuevo  ###########################################################

// en que posicion est� el pin sda
unsigned char i2cGetSdaRef(unsigned int sda, unsigned int* sda_pines) { 
	for (char n = 0 ; n < 4 ; n++) if( sda_pines[n] == sda ) return n;
} 
// #######################################   END  nuevo  ###########################################################
