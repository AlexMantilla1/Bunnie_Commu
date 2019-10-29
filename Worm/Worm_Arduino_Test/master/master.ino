
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
* + algo aqu� i2cSlaveSyncUpCopying(unsigned int* sdaPines) en BitBangI2C.cpp. Le puse un argumento m�s (modificar .h)
* + algo aqu� i2cSlaveListeningCopying(...) en SoftwareI2C.cpp, la syncAns que se retorna.
* + debo modificar el tiempo de sync a 6 ms
* + modifiqu� i2cMasterCheck para que retorne si encuentra un nuevo dispositivo. La convert� de void a unsigned int.
*   Agregu� 4 defines para respuetas. Agrego variable "message" (line 526) para llenarla en las l�neas 538,544,560,591 y retornarla en la 612
*/

//verificar esto!
/*
#define INPUT           1
#define OUTPUT          0
#define HIGH            1
#define LOW             0
*/


#include <SoftwareI2C.h>
#include <wormLib.h>

uint32_t T_us = 1000;
uint8_t sda = 0;
uint8_t scl = 14;
//uint8_t regAdressWrite = 43;
//uint8_t regAdressRead = 31;
uint8_t slaveAdress[] = {10 , 20 , 30};
//uint8_t dataReaded = 0;
//uint8_t dataToWrite = 86;
//uint8_t numSlaveFound = 0;

unsigned int sdaPines[4] = {1,5,7,4};
unsigned char dataStored[128];
unsigned int checkAns;
uint32_t order, dir;

void setup(){
  Serial.begin(9600);
  Serial.print("Master Started! ");
  //fill the memory
  for(int i = 0; i < 128 ; i++) dataStored[i] = 0x0;
  dataStored[MASTER_MEMORY_WORM_DELAY] = (unsigned char) DELAY_WORM_MS;
  dataStored[MASTER_MEMORY_WORM_SIZE] = 3;
  //dataStored[MASTER_MEMORY_WORM_DIR] = WORM_0_TO_8; //default

  i2cConfig( MASTER_MODE, sda, scl, T_us );
  i2cSetSlaveAddressesForMaster(&slaveAdress[0] , &dataStored[0]);
  //calc_vfuncionalidad();
}
// Master
void loop() {
  //Make the check for slave positions.
  checkAns = i2cMasterCheck( &dataStored[0], &sdaPines[0] );
  if ( checkAns == NEW_DEVICE_FOUND || checkAns == DEVICE_LOST ) {
      // Calculate new data and send data for slaves.
      sendDataToSlaves(calDelays(&dataStored[0]), &dataStored[0]);
      //cal the dir
      dataStored[MASTER_MEMORY_WORM_DIR] = calDirMaster(&dataStored[0], dataStored[MASTER_MEMORY_DELAY_B_WORMS]);
  }
  //end comu.
  endCommu();
  //Make worm aniamtion. Delays included.
  makeWorm( dataStored[MASTER_MEMORY_WORM_SIZE],
            dataStored[MASTER_MEMORY_WORM_DELAY],
            dataStored[MASTER_MEMORY_WORM_DIR],
            dataStored[MASTER_MEMORY_DELAY_B_WORMS] );
}
