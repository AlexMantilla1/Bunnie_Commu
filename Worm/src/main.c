#include <asf.h>
#include "ArduinoLike.h"
#include "ArduinoLike.c"
#include "BitBangI2C.h"
#include "BitBangI2C.cpp"
#include "SoftwareI2C.h"
#include "SoftwareI2C.cpp"
#include "wormLib.h"
#include "wormLib.Cpp"

/*
* Añadí:
* + algo aquí i2cSlaveSyncUpCopying(unsigned int* sdaPines) en BitBangI2C.cpp. Le puse un argumento más (modificar .h)
* + algo aquí i2cSlaveListeningCopying(...) en SoftwareI2C.cpp, la syncAns que se retorna.
* + debo modificar el tiempo de sync a 6 ms
* + modifiqué i2cMasterCheck para que retorne si encuentra un nuevo dispositivo. La convertí de void a unsigned int.
*   Agregué 4 defines para respuetas. Agrego variable "message" (line 526) para llenarla en las líneas 538,544,560,591 y retornarla en la 612
*/

//verificar esto!
#define INPUT           1
#define OUTPUT          0
#define HIGH            1
#define LOW             0

#define Masterboard
//#define Slave1board

#ifdef Masterboard
// Master
int main (void) {
    system_init();
    delay_init();
    micros_init(); //Init micros() and millis(), but use micros.
	
	uint16_t sdaPines[4] = {1,5,7,4};  //SAMD11
	uint8_t scl = 14;                      //SAMD11
    uint16_t T_us = 1000;
    uint8_t slaveAdress[] = {10 , 20 , 30};
    uint8_t dataStored[128];
    uint16_t checkAns;
    uint32_t order, dir;
    uint8_t sdaToFind[3] = {0,0,0};
	
	
    //fill the memory
    for(uint16_t i = 0; i < 128 ; i++) dataStored[i] = 0x0;
    dataStored[MASTER_MEMORY_WORM_DELAY] = (uint8_t) DELAY_WORM_MS;
    dataStored[MASTER_MEMORY_WORM_SIZE] = 3;
    dataStored[MASTER_MEMORY_WORM_DIR] = (uint8_t) WORM_0_TO_8; //default

    i2cConfig( MASTER_MODE, sdaPines[0], scl, T_us );
    i2cSetSlaveAddressesForMaster(&slaveAdress[0] , &dataStored[0]);

    //loop()
    while (1) {
        //Make the check for slave positions.
        checkAns = i2cMasterCheck( &dataStored[0], &sdaPines[0], &sdaToFind[0] );
        if ( checkAns == NEW_DEVICE_FOUND || checkAns == DEVICE_LOST ) {
	        // Calculate new data and send data for slaves.

	        //printMemory2(22);
	        sendDataToSlaves(calDelays(&dataStored[0]), &dataStored[0], &sdaToFind[0], &sdaPines[0]);
	        //cal the dir
	        dataStored[MASTER_MEMORY_WORM_DIR] = calDirMaster(&dataStored[0]);

        }
        //end comu.
        endCommu();
        //Make worm aniamtion. Delays included.
        makeWorm( dataStored[MASTER_MEMORY_WORM_SIZE],
        dataStored[MASTER_MEMORY_WORM_DELAY],
        dataStored[MASTER_MEMORY_WORM_DIR],
        dataStored[MASTER_MEMORY_DELAY_B_WORMS] );
    }
}

#else
#ifdef Slave1board
//Slave 1
int main (void) {
    system_init();
    delay_init();
    micros_init(); //Init micros() and millis(), but use micros.
	
    uint32_t slaveAddres = 10;        //Address for this slave.
    uint16_t sdaPines[4] = {1,5,7,4};  //SAMD11
    uint8_t scl = 14;                      //SAMD11
    //uint16_t sdaPines[4] = {2,3,4,5};    //Arduino
    //uint8_t scl = 7;                         //Arduino
    uint32_t timeout = 0xFFF;
    uint8_t dataStored[128];

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

    //loop()
    while (1) {
        slaveListeningState(timeout, &dataStored[0], &sdaPines[0]);
        dataStored[SLAVE_MEMORY_WORM_DIR] = calDirSlave(&dataStored[0]);
        makeWorm( dataStored[SLAVE_MEMORY_WORM_SIZE],
        dataStored[SLAVE_MEMORY_WORM_DELAY],
        dataStored[SLAVE_MEMORY_WORM_DIR],
        dataStored[SLAVE_MEMORY_DELAY_B_WORMS] );
    }
}

#else
//Slave 2
int main (void) {
    system_init();
    delay_init();
    uint32_t slaveAddres = 20;        //Address for this slave.
    uint16_t sdaPines[4] = {1,5,7,4};  //SAMD11
    uint8_t scl = 14;                      //SAMD11
    //uint16_t sdaPines[4] = {2,3,4,5};    //Arduino
    //uint8_t scl = 7;                         //Arduino
    uint32_t timeout = 0xFFF;
    uint8_t dataStored[128];

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

    //loop()
    while (1) {
	    slaveListeningState(timeout, &dataStored[0], &sdaPines[0]);
	    dataStored[SLAVE_MEMORY_WORM_DIR] = calDirSlave(&dataStored[0]);
	    makeWorm( dataStored[SLAVE_MEMORY_WORM_SIZE],
	    dataStored[SLAVE_MEMORY_WORM_DELAY],
	    dataStored[SLAVE_MEMORY_WORM_DIR],
	    dataStored[SLAVE_MEMORY_DELAY_B_WORMS] );
    }
}


#endif
#endif