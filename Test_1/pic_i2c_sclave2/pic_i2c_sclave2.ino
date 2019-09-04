#include <SoftwareI2C.h>

uint8_t numero_esclavo=3;
uint8_t dataStored[100];
uint32_t Addres = numero_esclavo*10;
uint8_t sda[] = {2, 3, 4, 5};
uint8_t scl = 7;

int z = numero_esclavo - 1;


void setup() {
  Serial.begin(9600);
  for(int i = 0; i < 100 ; i++) dataStored[i] = 0x1A;
  i2cConfig(SLAVE_MODE, sda[z], scl, Addres);
  dataStored[0]=sda[z];
  //Serial.println(dataStored[0],DEC);
  delay(1000);
}

void loop() {
  if(!i2cSlaveListening(100000,&dataStored[0])){
    if(!i2cSlaveListening(100000,&dataStored[0])){
    Serial.print("Connected as slave");
    Serial.print(" by the ports ");
    Serial.print(dataStored[0],DEC);
    Serial.print(" in slave and  ");
    Serial.print(dataStored[1],DEC);
    Serial.println(" in master. ");
    }
  }
}
