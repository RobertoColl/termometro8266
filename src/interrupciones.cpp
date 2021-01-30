#include "interrupciones.h"

//--Objetos externos
extern MCC_mqtt broker;

//--Variables extenas
extern uint8_t flag_seg;
extern uint8_t mqtt_conn_timeout;
extern uint8_t wifi_conn_timeout;
extern uint8_t mqtt_wait;
extern uint8_t live_timeout_mqtt;

//--Variables locales
uint8_t seg=0;
uint8_t minu=0;



void timer0_ISR (void) {
  flag_seg=~flag_seg;
  seg++;
  mqtt_conn_timeout++;
  wifi_conn_timeout++;
  //Serial.print("s:");Serial.println(seg);

  if(seg==60){
    seg=0;
    minu++;
    broker.mqtt_wait++;
    broker.live_timeout_mqtt++;
  
  }

  if(minu==60) minu=0;
  

  timer0_write(ESP.getCycleCount() + 80000000L); // 80MHz == 1sec
}