
#include <BitBangI2C.h>

// Global variables

uint16_t sdaPin = 1;			// Pin for sda, default: 1
uint16_t sclPin = 2;			// Pin for scl, default: 2

void i2cPinesDefine(uint16_t sda_Pin, uint16_t scl_Pin) {
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

void copySda(bool d, uint16_t sdaP) {
	pinMode(sdaP,OUTPUT);
	digitalWrite(sdaP,d);
}

void i2cSlaveSdasLow(uint16_t* sdaPines) {
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
uint16_t waitRiseFlank(uint8_t PIN, uint32_t timeout) {
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
uint16_t waitRiseFlankCopyingOne(uint8_t sclPIN, uint32_t timeout, uint16_t sdaToRead, uint16_t sdaToWrite) {
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
uint16_t waitRiseFlankCopyingAll(uint8_t PIN, uint32_t timeout, uint16_t* sdaPines) {
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
uint16_t waitFallFlank(uint8_t PIN, uint32_t timeout) {
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
uint16_t waitFallFlankCopyingOne(uint8_t sclPIN, uint32_t timeout, uint16_t sdaToRead, uint16_t sdaToWrite) {
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
uint16_t waitFallFlankCopyingAll(uint8_t PIN, uint32_t timeout, uint16_t* sdaPines) {
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
uint8_t i2cSlaveRead7Bits(uint32_t timeout) {
	uint8_t inByte = 0;
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

uint8_t i2cSlaveReadByte(uint32_t timeout) {
	uint8_t inByte = 0;
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

uint16_t i2cSlaveRead9Bits(uint32_t timeout) {
	uint16_t inByte = 0;
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

uint8_t i2cSlaveRead7BitsCopying(uint32_t timeout, uint16_t* sdaPines) {
	uint8_t inByte = 0;
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

uint8_t i2cSlaveReadByteCopying(uint32_t timeout, uint16_t* sdaPines) {
	uint8_t inByte = 0;
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

uint16_t i2cSlaveRead9BitsCopying(uint32_t timeout, uint16_t* sdaPines) {
	uint16_t inByte = 0;
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
void i2cSlaveWriteByte(uint8_t outByte, uint32_t timeout) {
	for (int i = 0; i < 8; ++i) {
		if( outByte & 0x80 ) i2cSlaveHighSda();  //msbit first
		else i2cSlaveLowSda();
		outByte = outByte << 1;
		waitFallFlank( sclPin, timeout );
	}
	i2cSlaveHighSda();
}

void i2cSlaveWrite9Bits(uint16_t outByte, uint32_t timeout) {
	for (int i = 0; i < 9; ++i) {
		if( outByte & 0x100 ) i2cSlaveHighSda();  //msbit first
		else i2cSlaveLowSda();
		outByte = outByte << 1;
		waitFallFlank( sclPin, timeout );
	}
	i2cSlaveHighSda();
}

// This function reads a byte from sda port(connected to slave) and puts it to sdaPin(connected to master)
uint16_t i2cSlaveWriteByteCopying(uint16_t slaveSda, uint32_t timeout) {
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
uint16_t i2cSlaveWrite9BitsCopying(uint16_t slaveSda, uint32_t timeout) {
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
void i2cSlaveNack(uint32_t timeout) {	//Signal For a correct recieved message
	i2cSlaveLowSda();
	waitFallFlank( sclPin, timeout );
	i2cSlaveHighSda();
}

// This Slave reads all his sda ports looking for a NACK from other slave. If a NACK is recieved, send a NACK to the master
// and returns the sdaPines' position where the NACK was found
uint16_t i2cSlaveNackFromSlave(uint32_t timeout, uint16_t* sdaPines) {	//Signal For a correct recieved message
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

uint16_t i2cSlaveNackFromOneSlave(uint32_t timeout, uint16_t sdaSlave) {	//Signal For a correct recieved message
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
void i2cSlaveAck(uint32_t timeout) {	//Signal For an incorrect message recieved
	i2cSlaveHighSda();
	waitFallFlank( sclPin, timeout );
	i2cSlaveHighSda();
}

// ******************* START AND STOP SLAVE CHECKING FUNCTIOINS  *********************************

//wait for the start condition while a given timeout
bool i2cSlaveWaitStartCondition(uint32_t timeout) {
	i2cSlaveHighScl();
	i2cSlaveHighSda();
	if( waitFallFlank(sdaPin, timeout) == RISE_OR_FALL_FLANK_OK ) {
		if( waitFallFlank(sclPin, timeout) == RISE_OR_FALL_FLANK_OK ) return 1;
		else return 0;
	} else return 0;
}
//wait for the start condition, copying the sdaPin(connected to master) to all ports from sdaPines array, while a given timeout
bool i2cSlaveWaitStartConditionCopying(uint32_t timeout, uint16_t* sdaPines) {
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
bool i2cSlaveWaitStopCondition(uint32_t timeout) {
	if( waitRiseFlank( sclPin, timeout ) == RISE_OR_FALL_FLANK_OK ) {
		if( waitRiseFlank( sdaPin, timeout ) == RISE_OR_FALL_FLANK_OK ) return 1;
		else return 0;
	} else return 0;
}
//wait for the stop condition, copying the sdaPin(connected to master) to all ports from sdaPines array, while a given timeout
bool i2cSlaveWaitStopConditionCopying(uint32_t timeout, uint16_t* sdaPines) {
	if( waitRiseFlankCopyingAll( sclPin, timeout, sdaPines ) == RISE_OR_FALL_FLANK_OK ) {
		if( waitRiseFlankCopyingAll( sdaPin, timeout, sdaPines ) == RISE_OR_FALL_FLANK_OK ) return 1;
		else return 0;
	} else return 0;
}

// ******************* SLAVE ADDITIONAL PROTOCOL FUNCTION *****************************

//Read only 1 bit from master
bool i2cSlaveReadBit(uint32_t timeout) {
	i2cSlaveHighSda();
	waitRiseFlank( sclPin, timeout );
	bool bit = digitalRead(sdaPin);
	waitFallFlank( sclPin, timeout );
	return bit;
}

//Read 1 bit from sdaPin (connected to Master) and copies this bit to the sdaSlave port (connected to Slave)
bool i2cSlaveReadBitCopying(uint32_t timeout, uint16_t sdaSlave) {
	i2cSlaveHighSda();
	waitRiseFlankCopyingOne( sclPin, timeout, sdaPin, sdaSlave );
	bool bit = digitalRead(sdaPin);
	waitFallFlankCopyingOne( sclPin, timeout, sdaPin, sdaSlave );
	return bit;
}

// #######################################   END  SLAVE   FUNCTIONS   ###########################################################

// #############################################   MASTER   FUNCTIONS   ##########################################################

// ******************* SDA, SCL MASTER CONTROL *********************************
void i2cMasterHighSda(uint16_t wait_us) {
   pinMode(sdaPin, INPUT); //SDA_DIR = 1;   // bring SDA to high impedance
   delayMicroseconds(wait_us); //delayMicroseconds(wait_us);//delayUs(5);
}

void i2cMasterLowSda(uint16_t wait_us) {
   digitalWrite(sdaPin, LOW);
   pinMode(sdaPin, OUTPUT);		// output a logic zero
   delayMicroseconds(wait_us); 				//delayMicroseconds(wait_us);//delayUs(5);
}

void i2cMasterHighScl(uint16_t wait_us) {
   pinMode(sclPin, INPUT);		// bring SCL to high impedance
   delayMicroseconds(wait_us); 				//delayMicroseconds(wait_us);//delayUs(5);
}

void i2cMasterLowScl(uint16_t wait_us) {
   digitalWrite(sclPin, LOW);
   pinMode(sclPin, OUTPUT);
   delayMicroseconds(wait_us);				//delayMicroseconds(wait_us);//delayUs(5);
}

// ******************* READ AND WRITE BYTE MASTER FUNCTIOINS  *********************************


void i2cMasterWrite7Bits(uint16_t wait_us, uint16_t out7Bits) {
	uint8_t n;
	for(n=0; n<7; n++) {
		if(out7Bits&0x40) i2cMasterHighSda(wait_us);
		else i2cMasterLowSda(wait_us);
		i2cMasterHighScl(wait_us);
		i2cMasterLowScl(wait_us);
		out7Bits = out7Bits << 1;
	}
	i2cMasterHighSda(wait_us);
}

void i2cMasterWriteByte(uint16_t wait_us, uint16_t outByte) {
	uint8_t n;
	for(n=0; n<8; n++) {
		if(outByte&0x80) i2cMasterHighSda(wait_us);
		else i2cMasterLowSda(wait_us);
		i2cMasterHighScl(wait_us);
		i2cMasterLowScl(wait_us);
		outByte = outByte << 1;
	}
	i2cMasterHighSda(wait_us);
}

void i2cMasterWrite9Bits(uint16_t wait_us, uint16_t out9Bits) {
	uint8_t n;
	for(n=0; n<9; n++) {
		if(out9Bits&0x100) i2cMasterHighSda(wait_us);
		else i2cMasterLowSda(wait_us);
		i2cMasterHighScl(wait_us);
		i2cMasterLowScl(wait_us);
		out9Bits = out9Bits << 1;
	}
	i2cMasterHighSda(wait_us);
}

uint8_t i2cMasterRead7Bits(uint16_t wait_us) {
	uint8_t inByte =0 , n;
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

uint8_t i2cMasterReadByte(uint16_t wait_us) {
	uint8_t inByte =0 , n;
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

uint16_t i2cMasterRead9Bits(uint16_t wait_us) {
	uint16_t inByte =0 , n;
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

void i2cMasterNack(uint16_t wait_us) {
   i2cMasterLowSda(wait_us);
   i2cMasterHighScl(wait_us);
   i2cMasterLowScl(wait_us);    // bring data high and clock
}

void i2cMasterAck(uint16_t wait_us) {
   i2cMasterLowSda(wait_us);
   i2cMasterHighScl(wait_us);
   i2cMasterLowScl(wait_us);
   i2cMasterHighSda(wait_us);    // bring data low and clock
}

// ******************* START AND STOP MASTER SET FUNCTIOINS  *********************************

void i2cMasterStart(uint16_t wait_us) {
	i2cMasterHighSda(wait_us);
	i2cMasterHighScl(wait_us);
	i2cMasterLowSda(wait_us);
	i2cMasterLowScl(wait_us);   // bring SDA low while SCL is high
}

void i2cMasterStop(uint16_t wait_us) {
	digitalWrite(sdaPin, LOW);
	pinMode(sdaPin,OUTPUT);
	i2cMasterLowScl(wait_us);
	i2cMasterLowSda(wait_us);
	i2cMasterHighScl(wait_us);
	i2cMasterHighSda(wait_us);    // bring SDA high while SCL is high
}

// ******************* MASTER ADDITIONAL PROTOCOL FUNCTION *****************************

// The Master reads one single bit.
bool i2cMasterReadBit(uint16_t wait_us) {
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
uint16_t setParityBit(uint8_t data, uint8_t dataBits) {
	uint8_t data1;
	uint16_t count = 0;
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
bool checkParity(uint16_t data) {
	uint16_t count = 0;
	while (data) {
		count += data & 1;
		data >>= 1;
	}
	count = (count & 1 ? 0 : 1);
	return count;
}

//This funciton removes the parity bit (MSB)
uint8_t removeParity(uint16_t parityData, uint16_t dataBits) {
	return (parityData & ~(0x1 << (dataBits-1)));
}

// #######################################   END  PARITY   FUNCTIONS   ###########################################################

// #######################################   SYNCHRONIZATION   FUNCTIONS   ###########################################################

// Master Sync Up
void i2cMasterSyncUp(uint16_t wait_us) {
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
uint16_t i2cSlaveSyncUp(uint32_t timeout) {
	pinMode(sclPin,INPUT);
	uint32_t i = timeout << 1;
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
uint16_t i2cSlaveSyncUpCopying(uint32_t timeout, uint16_t* sdaPines) {

	pinMode(sclPin,INPUT);
	bool flag = true; // flag to avoid to be setting 0 in SDAs every cicle.
	uint32_t i = timeout << 1;
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
uint8_t i2cGetSdaRef(uint16_t sda, uint16_t* sda_pines) {
	for (char n = 0 ; n < 4 ; n++) if( sda_pines[n] == sda ) return n;
}
// #######################################   END  nuevo  ###########################################################
