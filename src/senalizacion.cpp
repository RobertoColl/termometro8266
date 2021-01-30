#include "senalizacion.h"

//--Objetos externos
extern MCC_wifi wifi;
//extern MCC_mqtt broker;

//--Variables externas
extern uint8_t flag_seg;
extern uint8_t led_pulso;


void leds(void){
    //--WiFi
    wifi.setLed();
    //--Pulso
    flag_seg == 0 ? digitalWrite(led_pulso,HIGH):digitalWrite(led_pulso,LOW);//digitalWrite(LED, !digitalRead(LED));
    //--MQTT

}
