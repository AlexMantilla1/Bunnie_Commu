#include <SoftwareI2C.h>

uint32_t T_us = 1000;        //scl period on microsegundos.
uint8_t sda = 2;              //Master talks by port 2.
uint8_t scl = 7;              //Master generate clock by port 7.

uint8_t regAdressWrite = 43;   // dirrection a escribir,con el puerto del master
uint8_t regAdressRead = 31;  // dirrection a leer el puerto del esclavo

uint8_t slaveAdress[] = {10 , 20 , 30 , 40}; //Possible slave add, used for this test: 20 & 40.

uint8_t dataReaded = 0; //Data readed from the slave.
uint8_t dataToWrite = 86; //Data to write from master to slave
int i = 0;

void setup() {
  Serial.begin(9600);
  i2cConfig( MASTER_MODE, sda, scl, T_us );
}
void loop() {

  /******* ALGORITHM: ********
     1. Send a transaction requests each 500ms by the sda port, waiting for response.
     2. Once a slave responds, read reg 0 and write reg 1 to identify ports.
     3. Change the slave address.
     4. Wait 2 seg and back to point 1.
  ********************************/
/*
    Serial.println("Lets Read!");
    if( i2cMasterRead( slaveAdress[i], regAdressRead, &dataReaded ) == I2C_OK ) {
      Serial.println("Master read ok\n Lets write!");
      if( i2cMasterWrite( slaveAdress[i], regAdressWrite, sda ) == I2C_OK ) {
        Serial.print("Connected to slave ");
        Serial.print(slaveAdress[i],DEC);
        Serial.print(" by the ports ");
        Serial.print(sda,DEC);
        Serial.print(" in master and  ");
        Serial.print(dataReaded,DEC);
        Serial.println(" in slave. ");
      }
      delay(2000);
    }
    delay(500);*/

  if( i2cMasterRead( slaveAdress[i], regAdressRead, &dataReaded ) == I2C_OK )  {
    Serial.print("Connected to slave ");
    Serial.print(slaveAdress[i], DEC);
    Serial.print(" by the ports ");
    Serial.print(sda, DEC);
    Serial.print(" in master and  ");
    Serial.print(dataReaded, DEC);
    Serial.println(" in slave. ");
  }
  delay(2000);
}
