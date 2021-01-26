#include "interrupciones.h"

//--Variables extenas
extern int flag_seg;
extern uint8_t mqtt_conn_timeout;
extern uint8_t wifi_conn_timeout;
//extern unsigned long live_timeout_mqtt;
extern uint8_t flag_read_analog;
extern uint8_t minu_ip;

//--Variables locales
int seg=0;
int minu=0;

//int minu_timeout_broker;

void timer0_ISR (void) {
  flag_seg=~flag_seg;
  flag_read_analog=~flag_read_analog;
  seg++;
  mqtt_conn_timeout++;
  wifi_conn_timeout++;
  //Serial.print("s:");Serial.println(seg);

  if(seg==60){
    ////live_timeout_wifi++;
    //live_timeout_mqtt++;
    ////minu_timeout_wifi++;
    ////minu_timeout_broker++;
    seg=0;
    minu++;
    minu_ip++;
  }

  if(minu==60) minu=0;
  

  timer0_write(ESP.getCycleCount() + 80000000L); // 80MHz == 1sec
}