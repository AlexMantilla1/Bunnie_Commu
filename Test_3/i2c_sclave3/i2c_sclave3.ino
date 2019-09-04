#include <SoftwareI2C.h>

uint8_t dataStored[100];          //Data register.
uint32_t slaveAddres = 10;        //Address for this slave.
unsigned int sda[] = {2, 3, 4, 5};     //Ports used as sda channels.
uint8_t scl = 7;                  //Port used as scl channel.

void setup() {
  Serial.begin(9600);
  for(int i = 0; i < 100 ; i++) dataStored[i] = 0x1A;
  i2cConfig(SLAVE_MODE, sda[0], scl, slaveAddres);
}

void loop() {

  /******* ALGORITHM: ********
   *  1. Check 1 by 1 all sda channels waiting a transaction
   *  2. Once the transaction was found, read the address, while is copy the data to his
   *      othres sda ports.
   *  3. Check if the address owns to this slave, if not, check the others sda ports.   
   *  4. Respond to the master, according point 3.
   *  5. Continue the comm copying the data to the sda port as a wire.   
   ********************************/
  
	//i2cSlaveListeningCopying(unsigned int timeout, unsigned char* dataStored, unsigned int* sdaPines)
	if( i2cSlaveListeningCopying(100000, &dataStored[0], &sda[0]) ) {
		if( i2cSlaveListeningCopying(100000, &dataStored[0], &sda[0]) ) {
			Serial.print("Connected as slave");
			Serial.print(" by the ports ");
			Serial.print(dataStored[0],DEC);
			Serial.print(" in slave and ");
			Serial.print(dataStored[1],DEC);
			Serial.println(" in master. ");
		}
	}
}


	
