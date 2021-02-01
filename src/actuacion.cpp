#include "actuacion.h"


//--Variables externas
extern uint8_t canal1_pin;
extern uint8_t canal2_pin;
extern uint8_t canal1_status;
extern uint8_t canal2_status;

void canal1_on(void){
    digitalWrite(canal1_pin, HIGH);
    canal1_status=1;
    publica_canales(1);
}

void canal1_off(void){
    digitalWrite(canal1_pin, LOW);
    canal1_status=0;
    publica_canales(1);
}

void canal2_on(void){
    digitalWrite(canal2_pin, HIGH);
    canal2_status=1;
    publica_canales(2);
}

void canal2_off(void){
    digitalWrite(canal2_pin, LOW);
    canal2_status=0;
    publica_canales(2);
}
