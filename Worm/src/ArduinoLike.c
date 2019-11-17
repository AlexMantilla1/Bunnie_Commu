// ArduinoLike.c

#include "ArduinoLike.h"

#define INPUT 1
#define OUTPUT 0
#define HIGH 1
#define LOW 0

const uint8_t Vmaster[]={1,3,4,2};
const uint8_t Vs1[]={5,6,7,8};
const uint8_t Vs2[]={12,9,10,11};
const uint8_t Vs3[]={15,13,14,1};
uint8_t vfuncionalidad[]={0,0,0,0};
uint8_t pins[]={5,1,4,10,9,7,13};

uint8_t dataStored[128];
	
uint32_t VARIANT_MCK;
static volatile uint32_t _ulTickCount=0 ;

void micros_init() {
	VARIANT_MCK=system_gclk_gen_get_hz(0);
}
unsigned long millis( void )
{
	// todo: ensure no interrupts
	return _ulTickCount ;
}


void delayMicroseconds(int time){
	delay_us(time);
}
void delay(int time){
	delay_ms(time);
}
void pinMode(int Pin, int Mode){
	struct port_config config_port_pin;
	port_get_config_defaults(&config_port_pin);
	if(Mode == 0){
		config_port_pin.direction = PORT_PIN_DIR_OUTPUT;
		if(Pin == 1)port_pin_set_config(PIN_PA05, &config_port_pin);
		if(Pin == 4)port_pin_set_config(PIN_PA14, &config_port_pin);
		if(Pin == 5)port_pin_set_config(PIN_PA15, &config_port_pin);
		if(Pin == 6)port_pin_set_config(PIN_PA28, &config_port_pin);
		if(Pin == 7)port_pin_set_config(PIN_PA30, &config_port_pin);
		if(Pin == 8)port_pin_set_config(PIN_PA31, &config_port_pin);
		if(Pin == 9)port_pin_set_config(PIN_PA24, &config_port_pin);
		if(Pin == 10)port_pin_set_config(PIN_PA25, &config_port_pin);
		if(Pin == 13)port_pin_set_config(PIN_PA02, &config_port_pin);
		if(Pin == 14)port_pin_set_config(PIN_PA04, &config_port_pin);
	}
	if(Mode == 1){
		config_port_pin.direction = PORT_PIN_DIR_INPUT;
		if(Pin == 1)port_pin_set_config(PIN_PA05, &config_port_pin);
		if(Pin == 4)port_pin_set_config(PIN_PA14, &config_port_pin);
		if(Pin == 5)port_pin_set_config(PIN_PA15, &config_port_pin);
		if(Pin == 6)port_pin_set_config(PIN_PA28, &config_port_pin);
		if(Pin == 7)port_pin_set_config(PIN_PA30, &config_port_pin);
		if(Pin == 8)port_pin_set_config(PIN_PA31, &config_port_pin);
		if(Pin == 9)port_pin_set_config(PIN_PA24, &config_port_pin);
		if(Pin == 10)port_pin_set_config(PIN_PA25, &config_port_pin);
		if(Pin == 13)port_pin_set_config(PIN_PA02, &config_port_pin);
		if(Pin == 14)port_pin_set_config(PIN_PA04, &config_port_pin);
	}
}
void digitalWrite(int Pin, int newState){
	if(newState == 1){
		if(Pin == 1)port_pin_set_output_level(PIN_PA05, true);
		if(Pin == 4)port_pin_set_output_level(PIN_PA14, true);
		if(Pin == 5)port_pin_set_output_level(PIN_PA15, true);
		if(Pin == 6)port_pin_set_output_level(PIN_PA28, true);
		if(Pin == 7)port_pin_set_output_level(PIN_PA30, true);
		if(Pin == 8)port_pin_set_output_level(PIN_PA31, true);
		if(Pin == 9)port_pin_set_output_level(PIN_PA24, true);			
		if(Pin == 10)port_pin_set_output_level(PIN_PA25, true);
		if(Pin == 13)port_pin_set_output_level(PIN_PA02, true);
		if(Pin == 14)port_pin_set_output_level(PIN_PA04, true);
	}
	if(newState == 0){
		if(Pin == 1)port_pin_set_output_level(PIN_PA05, false);
		if(Pin == 4)port_pin_set_output_level(PIN_PA14, false);
		if(Pin == 5)port_pin_set_output_level(PIN_PA15, false);
		if(Pin == 6)port_pin_set_output_level(PIN_PA28, false);
		if(Pin == 7)port_pin_set_output_level(PIN_PA30, false);
		if(Pin == 8)port_pin_set_output_level(PIN_PA31, false);
		if(Pin == 9)port_pin_set_output_level(PIN_PA24, false);
		if(Pin == 10)port_pin_set_output_level(PIN_PA25, false);
		if(Pin == 13)port_pin_set_output_level(PIN_PA02, false);
		if(Pin == 14)port_pin_set_output_level(PIN_PA04, false);
	}
}
int digitalRead(int Pin){
	int value = 0;
	if(Pin == 1)value = port_pin_get_input_level(PIN_PA05);
	if(Pin == 4)value = port_pin_get_input_level(PIN_PA14);
	if(Pin == 5)value = port_pin_get_input_level(PIN_PA15);
	if(Pin == 6)value = port_pin_get_input_level(PIN_PA28);
	if(Pin == 7)value = port_pin_get_input_level(PIN_PA30);
	if(Pin == 8)value = port_pin_get_input_level(PIN_PA31);
	if(Pin == 9)value = port_pin_get_input_level(PIN_PA24);
	if(Pin == 10)value = port_pin_get_input_level(PIN_PA25);
	if(Pin == 13)value = port_pin_get_input_level(PIN_PA02);
	if(Pin == 14)value = port_pin_get_input_level(PIN_PA04);
	if(value == true)value = HIGH;
	if(value == false)value = LOW;
	return value;
}

uint32_t millis2(const struct tc_module *const module_inst){
	return (tc_get_count_value(module_inst)/(system_gclk_gen_get_hz(0)*1000));
}

unsigned long micros( void )
{
	uint32_t ticks, ticks2;
	uint32_t pend, pend2;
	uint32_t count, count2;

	ticks2  = SysTick->VAL;
	pend2   = !!(SCB->ICSR & SCB_ICSR_PENDSTSET_Msk)  ;
	count2  = _ulTickCount ;

	do
	{
		ticks=ticks2;
		pend=pend2;
		count=count2;
		ticks2  = SysTick->VAL;
		pend2   = !!(SCB->ICSR & SCB_ICSR_PENDSTSET_Msk)  ;
		count2  = _ulTickCount ;
	} while ((pend != pend2) || (count != count2) || (ticks < ticks2));

	return ((count+pend) * 1000) + (((SysTick->LOAD  - ticks)*(1048576/(VARIANT_MCK/1000000)))>>20) ;
	// this is an optimization to turn a runtime division into two compile-time divisions and
	// a runtime multiplication and shift, saving a few cycles
}

uint8_t* exvector(uint8_t esc,uint8_t vect[4]){
	uint8_t vectout[4];
	for(int i=0;i<5;i++){
		vectout[i]=esc*vect[i];
	}
	return vectout;
}

uint8_t* sumvector(uint8_t vect1[4],uint8_t vect2[4]){
	uint8_t vectout[4];
	for(int i=0;i<5;i++){
		vectout[i]=vect1[i]+vect2[i];
	}
	return vectout;
}

void calc_vfuncionalidad(){
	for(int i=0;i<4;i++){
		uint8_t* vect_temp;
		switch (dataStored[i])
		{
			case 0x0:
			vect_temp=exvector(0,vect_temp);
			break;
			case 0x1F:
			vect_temp=exvector(i+1,Vmaster);
			break;
			case 10:
			vect_temp=exvector(i+1,Vs1);
			break;
			case 20:
			vect_temp=exvector(i+1,Vs2);
			break;
			case 30:
			vect_temp=exvector(i+1,Vs3);
			break;
			
		}
		*vfuncionalidad=sumvector(vfuncionalidad,vect_temp);
	}
}

void update_frame(int j){
	uint8_t temp=vfuncionalidad[j];
	for(int i=0;i<7;i++){
		uint8_t temp2=temp&1;
		if(temp2==0){
			pinMode(pins[i],OUTPUT);
			digitalWrite(pins[i],LOW);
			}else{
			pinMode(pins[i],INPUT);
		}
		temp=temp>>1;
	}
}
