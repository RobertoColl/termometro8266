#include "actuacion.h"


//--Variables externas
extern uint8_t canal1;
extern uint8_t canal2;
extern uint8_t flag_canal1;
extern uint8_t flag_canal2;

void canal1_on(void){
    digitalWrite(canal1, HIGH);
    flag_canal1=1;
}

void canal1_off(void){
    digitalWrite(canal1, LOW);
    flag_canal1=0;
}

void canal2_on(void){
    digitalWrite(canal2, HIGH);
    flag_canal2=1;
}

void canal2_off(void){
    digitalWrite(canal2, LOW);
    flag_canal2=0;
}
