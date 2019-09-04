#include <SoftwareI2C.h>

  uint8_t scl = 7;            //puerto en el maestro para el clock
  uint8_t sda[]={ 2, 3, 4, 5};// puerto en el maestro para el sda
  uint32_t periodo = 500;        //periodo del scl en microsegundos.
  
  uint8_t slaveAdress[] = {10 ,20 ,30 ,40};   //dirrecion para los 4 esclavos
  uint8_t regAdressWrite = 1;   // dirrection a escribir,con el puerto del master
  uint8_t regAdressRead = 0;  // dirrection a leer el puerto del esclavo  
  uint8_t dataReaded = 0; //valor leido, con el puerto sda del esclavo
  //uint8_t dataReaded2 = 0;
  int regAdress = 45;
  int newRegValue = 99;
  unsigned int slaveAdress1 = 10;
  unsigned int slaveAdress2 = 20;
  
  
  
void setup() {
	Serial.begin(9600);
  //i2cConfig(MASTER_MODE, sda[0], scl, periodo);
  //i2cConfig(MASTER_MODE, sda[0], scl, periodo);
  //i2cMasterStop();
}
int i =0;
int j=1;
void loop() {
 
  //for(int i = 0 ; i < 4 ; i++){
    i2cConfig(MASTER_MODE, sda[i], scl, periodo);
    //for(int j = 0; j < 4; j++){
      //for(int z = 0; z < 4; z++){
      Serial.println("Vamo a ecribi");
        //if(i2cMasterWrite(slaveAdress[j],regAdressWrite,sda[i])==0){
        if(i2cMasterRead(slaveAdress[j],regAdressRead,&dataReaded)==0){
          Serial.println("Master read ok\n Vamo a lee");
          //delay(1);
            //if(i2cMasterRead(slaveAdress[j],regAdressRead,&dataReaded)==0){
            if(i2cMasterWrite(slaveAdress[j],regAdressWrite,sda[i])==0){
              Serial.print("Connected to slave ");
              Serial.print(slaveAdress[j],DEC);
              Serial.print(" by the ports ");
              Serial.print(sda[i],DEC);
              Serial.print(" in master and  ");
              Serial.print(dataReaded,DEC);
              Serial.println(" in slave. ");
              //delay(1000);
              }
              delay(2000000);
        }
      //}<
    //}
  //}
 /*
	//start transmision
    	
	//read
	/*
	i2cMasterWrite(slaveAdress,regAdress,newRegValue);
	i2cMasterRead(slaveAdress,regAdress,&dataReaded);
	Serial.print("dataReaded: ");
	Serial.println(dataReaded, BIN);
	*/



  /*
  if(!i2cMasterWrite(slaveAdress1,regAdress,newRegValue)){
    Serial.println(slaveAdress1,DEC);
    Serial.println(regAdress,BIN);
    Serial.println(newRegValue,BIN);
  }*/
  /*
	for (int i = 0 ; i < 100 ; i++) {
	if( i2cMasterWrite(slaveAdress1,regAdress,newRegValue) ) {
		Serial.println("Erda! hubo un error en escritura!");
	} else {
		Serial.println("Yas! No hubo un error en escritura!");
		if(i2cMasterRead(slaveAdress1,regAdress,&dataReaded)) {
			Serial.println("Erda! hubo un error en lectura!");
		} else {
			Serial.println("Yas! No hubo un error en lectura!");
			if(dataReaded == newRegValue) {
			  Serial.println("Transacción exitosa!");
			}
		}
	}
	regAdress++;
	newRegValue++;
  
	delay(200);
	}
   */
   /*
   delay(50);
   if(i2cMasterRead(slaveAdress1,regAdress,&dataReaded1)) {
     Serial.println("Erda! hubo un error en lectura!");
    }else {
     Serial.println("Yas! No hubo un error en lectura!");
      if(dataReaded1 == newRegValue) {
        Serial.println("Transacción exitosa!");
      }
    }
    
   delay(100);
  if(!i2cMasterWrite(slaveAdress2,regAdress,newRegValue)){
    Serial.println(slaveAdress2,DEC);
    Serial.println(regAdress,BIN);
    Serial.println(newRegValue,BIN);
  }
  delay(50);
   if(i2cMasterRead(slaveAdress2,regAdress,&dataReaded2)) {
     Serial.println("Erda! hubo un error en lectura!");
    } else {
     Serial.println("Yas! No hubo un error en lectura!");
      if(dataReaded2 == newRegValue) {
        Serial.println("Transacción exitosa!");
      }
    }
  delay(100);
  */
}
    
