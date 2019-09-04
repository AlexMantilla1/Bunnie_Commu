
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

void i2cSlaveLowSda(void) {		// bring SDA to LOW
	digitalWrite(sdaPin,LOW); 
	pinMode(sdaPin,OUTPUT);	
}

void i2cSlaveHighScl(void) {
	pinMode(sclPin,INPUT); 		// bring SCL to high impedance 
}

// ******************* SLAVE NEEDS TO READ THE RISE AND FALL SCL/SDA FLANK *********************************

void waitRiseFlank(unsigned char PIN) {
	while(digitalRead(PIN));
	while(!digitalRead(PIN));
}

void waitFallFlank(unsigned char PIN) {
	while(!digitalRead(PIN));
	while(digitalRead(PIN));
}

// ******************* READ AND WRITE BYTE SLAVE FUNCTIOINS  *********************************
unsigned char i2cSlaveReadByte(void) {
	unsigned char inByte = 0;
	for (int i = 0; i < 8; ++i) {
		waitRiseFlank(sclPin);
		if (digitalRead(sdaPin)) {
			inByte = (inByte << 1) | 0x01; // msbit first
		} else {
			inByte = (inByte << 1);
		}
	}
	return inByte;
}

void i2cSlaveWriteByte(unsigned char outByte) {
	for (int i = 0; i < 8; ++i) {		
		if(outByte&0x80) i2cSlaveHighSda();  //msbit first
		else i2cSlaveLowSda();
		outByte = outByte << 1;
		waitFallFlank(sclPin);
	}
	//waitFallFlank(sclPin);
	i2cSlaveHighSda();
}

// ******************* ACK AND NACK SLAVE FUNCTIONS  *********************************

void i2cSlaveNack(void) {	//Signal For a correct recieved message
	i2cSlaveLowSda();
	waitFallFlank(sclPin);
	i2cSlaveHighSda();
}

void i2cSlaveAck(void) {	//Signal For an incorrect recieved message
	i2cSlaveHighSda();
}

// ******************* START AND STOP SLAVE CHECKING FUNCTIOINS  *********************************

//wait for the start condition while a given timeout
bool i2cSlaveWaitStartCondition(unsigned int timeout) { 
	i2cSlaveHighScl();
	i2cSlaveHighSda();
	if(digitalRead(sclPin) & digitalRead(sdaPin))
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
	else return 0;
}
//wait for the stop condition while a given timeout
bool i2cSlaveWaitStopCondition(unsigned int timeout) {
	//i2cSlaveHighScl();
	//i2cSlaveHighSda();
	waitRiseFlank(sclPin);
	waitRiseFlank(sdaPin);
	return 1;
	/*
	//while(1){
		while(digitalRead(sdaPin));
		while(!digitalRead(sdaPin)) {
			if(digitalRead(sclPin)) {
				while(!digitalRead(sdaPin)) {
					if(timeout == 0) return 0;
					timeout--;
				}
				return 1; 
			}
			if(timeout == 0) return 0;
			timeout--; 
		}
	//}
	*/
}

// ******************* SLAVE ADDITIONAL PROTOCOL FUNCTION *****************************

//Read only 1 bit
bool i2cSlaveReadBit() {
	i2cSlaveHighSda();
	waitRiseFlank(sclPin);
	if (digitalRead(sdaPin)) return 1;
	else return 0;
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

