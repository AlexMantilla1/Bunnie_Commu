
#include "wormLib.h"

// ############################################# For both #############################################
//Keep the Listening state until the end flag
//Setting the led
void setLed(uint8_t ledNum, uint8_t value) {
    uint8_t pinNumber = 0;
    switch(ledNum) {
        case 1:
            pinNumber = 14;
            break;
        case 2:
            pinNumber = 5;
          break;
        case 3:
            pinNumber = 1;
            break;
        case 4:
            pinNumber = 4;
            break;
        case 5:
            pinNumber = 10;
            break;
        case 6:
            pinNumber = 9;
            break;
        case 7:
            pinNumber = 7;
            break;
        case 8:
            pinNumber = 13;
            break;
        default:
            pinNumber = 0;
            break;
    }
    if(value) {
        digitalWrite(pinNumber,LOW);
        pinMode(pinNumber,OUTPUT);
    } else pinMode(pinNumber,INPUT);

}
// Making the worm go from led 0 to led 8.
void makeWormUp(uint32_t wormSize, uint32_t delayTime_ms) {
    int pos = 0;
    //Turn off all leds.
    for (int i = 0; i < 8; i++) setLed(i+1,OFF);
    //Build the worm
    for (int i = 0; i < wormSize; i++) {
        //digitalWrite(i,LOW);
        //pinMode(i,OUTPUT);
        setLed(i+1,ON);
        delay(delayTime_ms);
    }
    //Move the worm
    while(pos < (8 - wormSize)) {
        setLed(pos+1,OFF);
        //digitalWrite(pos+wormSize,LOW);
        //pinMode(pos+wormSize,OUTPUT);
        setLed(pos+wormSize+1,ON);
        pos++;
        delay(delayTime_ms);
    }
    //Destroy the worm
    for (int i = 0; i < wormSize; i++) {
        //pinMode(pos+i,INPUT);
        setLed(pos+i+1,OFF);
        delay(delayTime_ms);
    }
}
// Making the worm go from led 8 to led 0.
void makeWormDown(uint32_t wormSize, uint32_t delayTime_ms) {
    int pos = 7;
    //Turn off all leds.
    for (int i = 0; i < 8; i++) setLed(i+1,OFF); // pinMode(i,INPUT);
    //Build the worm
    for (int i = 7; i > 8 - wormSize; i--) {
        //digitalWrite(i,LOW);
        //pinMode(i,OUTPUT);
        setLed(i+1,ON);
        delay(delayTime_ms);
    }
    //Move the worm
    while(pos > wormSize - 1) {
        //pinMode(pos,INPUT);
        setLed(pos+1,OFF);
        //digitalWrite(pos-wormSize,LOW);
        //pinMode(pos-wormSize,OUTPUT);
        setLed(pos-wormSize+1,ON);
        pos--;
        delay(delayTime_ms);
    }
    //Destroy the worm
    for (int i = 0; i < wormSize; i++) {
        //pinMode(pos-i,INPUT);
        setLed(pos-i+1,OFF);
        delay(delayTime_ms);
    }
}
// Making the worm animation.
void makeWorm(uint32_t wormSize, uint32_t delayTime_ms, bool param, uint8_t waits) {
    unsigned int wormTime_ms = delayTime_ms*(8+wormSize);
    delay( (waits >> 4)*wormTime_ms );
    if (param) makeWormDown(wormSize,delayTime_ms);
    else makeWormUp(wormSize,delayTime_ms);
    delay( (waits & 0xF)*wormTime_ms );
}
// ############################################# END For both #############################################
// ############################################# For Master #############################################
// Check if the slave is connected directly to the master
bool slaveIsAtMasterSide(ref) {
    if (ref <= MASTER_MEMORY_DOWN) return true;
    else return false;
}
// Get the number of the slave imediatly between the slave and the master
uint8_t getSlaveAtSide(uint8_t ref) {
    if (ref < MASTER_MEMORY_S2_RIGHT) return 0;
    else if (ref < MASTER_MEMORY_S3_RIGHT) return 1;
    else return 2;
}
//Convert the orders to a 32 bits register with the delays for all the slaves and the master.
uint32_t convertOrderToDelays(uint8_t* order) {
    uint32_t reg = 0;
    for (uint8_t i = 0; i < 4; i++) {
        if(order[i] == 0) reg <<= 8;
        else {
            reg <<= 4;
            reg |= order[i] - 1;
            reg <<= 4;
            reg |= 4 - order[i];
        }
    }
    return reg;
}
//Calculate the Delays needed to syncronize the worm.
uint32_t calDelays( unsigned char* dataStored ) {
    // variables
    uint8_t num = 0; // slaves found
    uint8_t cnt = 0;
    bool slavesConnec[3];
    uint8_t slavesPos[3];
    uint8_t numSlavesToSlave[3];
    uint8_t order[4]; // Master is the fourth position.
    //Number of slaves found.
    for (uint8_t i = 0; i < 3; i++) {
        num += (dataStored[MASTER_MEMORY_S1_ADD+i] & 0x1);
        slavesConnec[i] = (dataStored[MASTER_MEMORY_S1_ADD+i] & 0x1);
    }
    //find the positions
    for (uint8_t i = 0; i < 3; i++) {
        uint8_t add = dataStored[MASTER_MEMORY_S1_ADD+i] >> 3;
        for (uint8_t j = 0; j < MASTER_MEMORY_S1_ADD; j++) if (dataStored[j] == add) slavesPos[i] = j;
    }
    //find the number of slaves between the master and each slave.
    for (uint8_t i = 0; i < 3; i++) {
        bool flag = false;
        uint8_t ref = slavesPos[i];
        if (slavesConnec[i]) {
            if ( slaveIsAtMasterSide(ref) ) {
                numSlavesToSlave[i] = 0;
                continue;
            }
            while (!flag) {
                cnt++;
                ref = slavesPos[getSlaveAtSide(ref)];
                if ( slaveIsAtMasterSide(ref) ) {
                    numSlavesToSlave[i] = cnt;
                    flag = true;
                }
            }
        }
    }
    // Find the order
    for (uint8_t i = 0; i < 3; i++) order[i] = 0;
    switch (num) {
        case 0: //the Master is alone.
            order[3] = 1;
            break;
        case 1: //1 Slave connected.
            order[3] = 1;
            for (uint8_t i = 0; i < 3; i++) if(slavesConnec[i] == true) order[i] = 2;
            break;
        case 2: //2 Slaves connected.
			;
            uint8_t temp = 3;
			uint8_t refTemp = 3;
            for (uint8_t i = 0; i < 3; i++)
                if(slavesConnec[i] == true) {
                    if (temp == 3) {
                        temp = slavesPos[i];
                        refTemp = i;
                    } else if (temp == slavesPos[i]) {
                        order[refTemp] = 1;
                        order[3] = 2;
                        order[i] = 3;
                    } else if (temp < slavesPos[i]){
                        order[i] = 1;
                        order[refTemp] = 2;
                        order[3] = 3;
                    } else {
                        order[refTemp] = 1;
                        order[i] = 2;
                        order[3] = 3;
                    }
                }
            break;
        case 3: //3 Slaves connected. //esta no funciona por ahora
            //line recta.
            if(slavesPos[0] > slavesPos[1]) {
                if(slavesPos[0] > slavesPos[2]) {
                    order[0] = 1;
                    if (slavesPos[1] > slavesPos[2]) {
                        order[1] = 2;
                        order[2] = 3;
                        order[3] = 4;
                    } else {
                        order[2] = 2;
                        order[1] = 3;
                        order[3] = 4;
                    }
                } else {
                    order[2] = 1;
                    order[0] = 2;
                    order[1] = 3;
                    order[3] = 4;
                }
            } else if (slavesPos[1] > slavesPos[2]) {
                order[1] = 1;
                if (slavesPos[0] > slavesPos[2]) {
                    order[0] = 2;
                    order[2] = 3;
                } else {
                    order[2] = 2;
                    order[0] = 3;
                }
            }
            // other weird things.
            /*
            uint8_t temp1 = 3, temp2 = 3, refTemp1 = 3, refTemp2 = 3;
            for (uint8_t i = 0; i < 3; i++)
                if(slavesConnec[i] == true) {
                    if (temp1 == 3) {
                        temp1 = slavesPos[i];
                        refTemp1 = i;
                    } else if (temp2 == 3) {
                        temp2 = slavesPos[i];
                        refTemp2 = i;
                    } else if (temp1 == slavesPos[i]) {
                        order[refTemp] = 1;
                        order[3] = 2;
                        order[i] = 3;
                    } else if (temp < slavesPos[i]){
                        order[i] = 1;
                        order[refTemp] = 2;
                        order[3] = 3;
                    } else {
                        order[refTemp] = 1;
                        order[i] = 2;
                        order[3] = 3;
                    }
                }
            */
            break;
        default:
            order[3] = 1;
            break;
    }
    return convertOrderToDelays(&order[0]);
}
//Send the data to the slaves con configure the worm secuency.
void sendDataToSlaves(uint32_t delays, unsigned char* dataStored) {
    for (uint8_t i = 0; i < 4; i++) {
        uint8_t data = delays >> (24 - i*8);
        data |= 0xFF;
        if(i < 3) while( i2cMasterWrite( (dataStored[MASTER_MEMORY_S1_ADD+i] >> 3 ), SLAVE_MEMORY_DELAY_B_WORMS, data ) != I2C_OK );
        else dataStored[MASTER_MEMORY_DELAY_B_WORMS] = data;
    }
}
//Make the flag to end the communication.
void endCommu() {
    delayMicroseconds(SINC_TIME_MASTER_US);
    // put low scl and sdas!
    setLed(1,ON);
    setLed(2,ON);
    setLed(3,ON);
    setLed(7,ON);
    delayMicroseconds(SINC_TIME_MASTER_US*2);
    // put high scl!
    setLed(1,OFF);
    setLed(2,OFF);
    setLed(3,OFF);
    setLed(7,OFF);
}
//Calculates the dir for the worm in master.
bool calDirMaster(unsigned char* dataStored, uint32_t data) {
    uint8_t num = 0; //number of devices connected
    uint8_t slaveRefs[3] = {4,4,4};
    uint8_t order = (data >> 4) + 1;
    bool dir = WORM_0_TO_8; //0 or false.

    for (uint8_t i = 0; i < 3; i++) {
        for (uint8_t j = 0; j < 4; j++)
            if(dataStored[j] == (dataStored[MASTER_MEMORY_S1_ADD+i] >> 3)) {
                num++;
                slaveRefs[i] = j;
            }
    }
    switch (num) {
        case 1:
            if (slaveRefs[0] == 3 || slaveRefs[1] == 3 || slaveRefs[2] == 3) dir = WORM_8_TO_0;
            break;
        case 2:
            if (slaveRefs[0] == 3 || slaveRefs[1] == 3 ) dir = WORM_8_TO_0;
            else if (slaveRefs[0] == 1 || slaveRefs[1] == 1) dir = WORM_0_TO_8;
            else if (slaveRefs[0] == 0 && (slaveRefs[1] == 1 || slaveRefs[2] == 1)) dir = WORM_8_TO_0;
            else if (slaveRefs[1] == 0 && (slaveRefs[2] == 1)) dir = WORM_8_TO_0;
            break;
        case 3:
            //not cosidered yet.
            break;
        default:
            break;
    }
    return dir;
}
// ############################################# END For Master #############################################
// ############################################# For slave #############################################
//Keep the Listening state until the end flag
void slaveListeningState(unsigned long timeout, unsigned char* dataStored, unsigned int* sdaPines) {
    bool endFlag = false;
    delayMicroseconds(SINC_TIME_SLAVE_US >> 1);
    while (!endFlag) if (i2cSlaveListeningCopying(timeout, dataStored, sdaPines) == END_COMM) endFlag = true;
}
//Wait a syncronization flag to start the worm animation
void waitSleep(uint8_t sclPin) {
    waitRiseFlank(sclPin, 0xFFFFFFFF);
}
//Calculates the dir for the worm in slaves.
bool calDirSlave(unsigned char* dataStored, uint32_t data) {
    uint8_t num = 0; //number of devices connected
    uint8_t masterRef = 4;
    uint8_t SlaveRef1 = 4;
    uint8_t SlaveRef2 = 4;
    uint8_t order = (data >> 4) + 1;
    bool dir = WORM_8_TO_0; //0 or false.

    for (uint8_t i = 0; i < 4; i++) {
        if(dataStored[i] != 0) {
            num++;
            if(dataStored[i] == 0x1F) masterRef = i;
            else if(SlaveRef1 == 4) SlaveRef1 = i;
            else SlaveRef2 = i;
        }
    }
    switch (num) {
        case 1:
            if (masterRef > 1) dir = WORM_0_TO_8;
            break;
        case 2:
            if (SlaveRef1 > 1) dir = WORM_8_TO_0;
            else if (masterRef > 1 || SlaveRef1 == 1) dir = WORM_0_TO_8;
            else if (SlaveRef1 == 1) dir = WORM_8_TO_0;
            break;
        case 3:
            //not cosidered yet.
            break;
        default:
            break;
    }
    return dir;
}
// ############################################# END For slave #############################################