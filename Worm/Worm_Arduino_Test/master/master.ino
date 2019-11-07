//#define SAMD11
#define ARDUINO_LEDS


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

*/
#ifdef SAMD11
#include <asf.h>
#include "ArduinoLike.c"
#include "ArduinoLike.h"
#include "BitBangI2C.cpp"
#include "BitBangI2C.h"
#include "SoftwareI2C.cpp"
#include "SoftwareI2C.h"
#include "wormLib.h"
#include "wormLib.C"
#define INPUT           1
#define OUTPUT          0
#define HIGH            1
#define LOW             0

uint16_t sdaPines[4] = {1,5,7,4};  //SAMD11
uint8_t scl = 14;                      //SAMD11
#endif

#ifdef ARDUINO_LEDS
#include <SoftwareI2C.h>
#include <wormLib.h>

uint16_t sdaPines[4] = {2,3,4,5};    //Arduino
uint8_t scl = 7;                         //Arduino

#endif

uint16_t T_us = 1000;
uint8_t slaveAdress[] = {10 , 20 , 30};
uint8_t dataStored[128];
uint16_t checkAns;
uint32_t order, dir;
uint8_t sdaToFind[3] = {0,0,0};

#ifdef ARDUINO_LEDS
void printMemory2(int hasta) {
  Serial.println("Memory:");
  for( int i = 0; i < (hasta + 1); i++) {
    Serial.print(i,DEC); Serial.print(". "); Serial.println(dataStored[i],BIN);
  }
}
#endif

void setup(){
  //for(char i = 0; i < 4; i++) digitalWrite(sdaPins[i],LOW);
  #ifdef ARDUINO_LEDS
  Serial.begin(9600);
  Serial.println("Master Started! ");
  #endif
  //fill the memory
  for(uint16_t i = 0; i < 128 ; i++) dataStored[i] = 0x0;
  dataStored[MASTER_MEMORY_WORM_DELAY] = (uint8_t) DELAY_WORM_MS;
  dataStored[MASTER_MEMORY_WORM_SIZE] = 3;
  dataStored[MASTER_MEMORY_WORM_DIR] = (uint8_t) WORM_0_TO_8; //default

  i2cConfig( MASTER_MODE, sdaPines[0], scl, T_us );
  i2cSetSlaveAddressesForMaster(&slaveAdress[0] , &dataStored[0]);
  //calc_vfuncionalidad();
}
// Master
void loop() {
  //Make the check for slave positions.
  //
  Serial.println("1");
  checkAns = i2cMasterCheck( &dataStored[0], &sdaPines[0], &sdaToFind[0] );
  if ( checkAns == NEW_DEVICE_FOUND || checkAns == DEVICE_LOST ) {
    Serial.println("2");
      // Calculate new data and send data for slaves.

      printMemory2(22);
      sendDataToSlaves(calDelays(&dataStored[0]), &dataStored[0], &sdaToFind[0], &sdaPines[0]);
      Serial.println("2.5");
      //cal the dir
      dataStored[MASTER_MEMORY_WORM_DIR] = calDirMaster(&dataStored[0], dataStored[MASTER_MEMORY_DELAY_B_WORMS]);

  }
  Serial.println("3");
  //end comu.
  endCommu();
  Serial.println("4");
  //Make worm aniamtion. Delays included.
  makeWorm( dataStored[MASTER_MEMORY_WORM_SIZE],
            dataStored[MASTER_MEMORY_WORM_DELAY],
            dataStored[MASTER_MEMORY_WORM_DIR],
            dataStored[MASTER_MEMORY_DELAY_B_WORMS] );
  Serial.println("5");
  /*
    pinMode(7,OUTPUT);
    digitalWrite(7,LOW);
    printMemory2(22);
    delay(123947824);
    */
}
