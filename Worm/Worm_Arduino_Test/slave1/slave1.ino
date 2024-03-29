
/*#include <asf.h>
#include "ArduinoLike.c"
#include "ArduinoLike.h"
#include "BitBangI2C.cpp"
#include "BitBangI2C.h"
#include "SoftwareI2C.cpp"
#include "SoftwareI2C.h"

#include "wormLib.h"
#include "wormLib.C"
*/
/*
* A�ad�:
* + algo aqu� i2cSlaveSyncUpCopying(uint16_t* sdaPines) en BitBangI2C.cpp. Le puse un argumento m�s (modificar .h)
* + algo aqu� i2cSlaveListeningCopying(...) en SoftwareI2C.cpp, la syncAns que se retorna.
* + debo modificar el tiempo de sync a 6 ms
* + modifiqu� i2cMasterCheck para que retorne si encuentra un nuevo dispositivo. La convert� de void a uint16_t.
*   Agregu� 4 defines para respuetas. Agrego variable "message" (line 526) para llenarla en las l�neas 538,544,560,591 y retornarla en la 612
*/

//verificar esto!
/*
#define INPUT           1
#define OUTPUT          0
#define HIGH            1
#define LOW             0
*/

#include <wormLib.h>
#include <SoftwareI2C.h>

uint32_t slaveAddres = 10;        //Address for this slave.
//uint16_t sdaPines[4] = {1,5,7,4};  //SAMD11
//uint8_t scl = 14;                      //SAMD11
uint16_t sdaPines[4] = {2,3,4,5};    //Arduino
uint8_t scl = 7;                         //Arduino
uint32_t timeout = 0xFFF;
uint8_t dataStored[128];

void printMemory2(int hasta) {
  Serial.println("Memory:");
  for( int i = 0; i < (hasta + 1); i++) {
    Serial.print(i,DEC); Serial.print(". "); Serial.println(dataStored[i],BIN);
  }
}

void setup() {
  Serial.begin(9600);
  Serial.print("Slave Started! "); Serial.println(slaveAddres,BIN);
  //fill the memory
  for(int i = 0; i < 128 ; i++) dataStored[i] = 0x0;
  dataStored[SLAVE_MEMORY_WORM_DELAY] = (uint8_t) DELAY_WORM_MS;
  dataStored[SLAVE_MEMORY_WORM_SIZE] = 3;
  //dataStored[SLAVE_MEMORY_WORM_DIR] = WORM_0_TO_8;
  //Slave Config
  i2cConfig(SLAVE_MODE, sdaPines[0], scl, slaveAddres);
  //turnOffFlag(WAITING_FLAG);
  //Slave Set up
  i2cSlaveSetUp(timeout, &dataStored[0], &sdaPines[0]);
}
// Slave1
void loop() {
  //Make the check for slave positions.

  slaveListeningState(timeout, &dataStored[0], &sdaPines[0]);
        dataStored[SLAVE_MEMORY_WORM_DIR] = calDirSlave(&dataStored[0]);
        makeWorm( dataStored[SLAVE_MEMORY_WORM_SIZE],
        dataStored[SLAVE_MEMORY_WORM_DELAY],
        dataStored[SLAVE_MEMORY_WORM_DIR],
        dataStored[SLAVE_MEMORY_DELAY_B_WORMS] );
}
