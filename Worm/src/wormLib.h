#ifndef WORM_LIB
#define WORM_LIB


//#define Arduino_LEDS
#define SAMD11

#ifdef Arduino_LEDS
#include <stdint.h>
#include <Arduino.h>
#include <SoftwareI2C.h>
#endif
#ifdef SAMD11
#include "ArduinoLike.h"
#endif

//Some Values:
#define DELAY_WORM_MS           0xFF
#define SINC_TIME_MASTER_MS     30
#define SINC_TIME_SLAVE_MS      60
//Some instructions:
#define ON              1
#define OFF             0
#define WORM_0_TO_8     0
#define WORM_8_TO_0     1
//Memory use:
#define MASTER_MEMORY_DELAY_B_WORMS     19
#define MASTER_MEMORY_WORM_DELAY        20
#define MASTER_MEMORY_WORM_SIZE         21
#define MASTER_MEMORY_WORM_DIR          22

#define SLAVE_MEMORY_DELAY_B_WORMS      7
#define SLAVE_MEMORY_WORM_DELAY         8
#define SLAVE_MEMORY_WORM_SIZE          9
#define SLAVE_MEMORY_WORM_DIR           10

// ############################################# For both #############################################
//Keep the Listening state until the end flag
//Setting the led
void setLed(uint8_t ledNum, uint8_t value);
// Making the worm go from led 0 to led 8.
void makeWormUp(uint32_t wormSize, uint32_t delayTime_ms);
// Making the worm go from led 8 to led 0.
void makeWormDown(uint32_t wormSize, uint32_t delayTime_ms);
// Making the worm animation.
void makeWorm(uint32_t wormSize, uint32_t delayTime_ms, bool param, uint8_t waits);
// ############################################# END For both #############################################
// ############################################# For Master #############################################
// Check if the slave is connected directly to the master
bool slaveIsAtMasterSide(uint8_t ref);
// Get the number of the slave imediatly between the slave and the master
uint8_t getSlaveAtSide(uint8_t ref);
//Convert the orders to a 32 bits register with the delays for all the slaves and the master.
uint32_t convertOrderToDelays(uint8_t* order, uint8_t num);
//Calculate the Delays needed to syncronize the worm.
uint32_t calDelays( uint8_t* dataStored );
//Send the data to the slaves con configure the worm secuency.
void sendDataToSlaves(uint32_t delays, uint8_t* dataStored, uint8_t* sdaToFind, uint16_t* sdaPines);
//Make the flag to end the communication.
void endCommu();
//Calculates the dir for the worm in master.
bool calDirMaster(uint8_t* dataStored);
// ############################################# END For Master #############################################
// ############################################# For slave #############################################
//Keep the Listening state until the end flag
void slaveListeningState(uint32_t timeout, uint8_t* dataStored, uint16_t* sdaPines);
//Wait a syncronization flag to start the worm animation
void waitSleep(uint8_t sclPin);
//Calculates the dir for the worm in slaves.
bool calDirSlave(uint8_t* dataStored);
// ############################################# END For slave #############################################

#endif