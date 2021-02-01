#include "publica.h"

//--Objetos externos
extern MCC_mqtt broker;

//--Variables externas
extern const char * topic_telemetry;
extern uint8_t canal1_status;
extern uint8_t canal2_status;

//--Variables locales
uint16_t cont_int_pub=0;
StaticJsonDocument<200> telem;
char json_telem[200];
uint8_t flag_push=0;


void send_pub(void){
    serializeJson(telem, json_telem);
    Serial.print("Publica:");
    Serial.println(json_telem);
    broker.pub(topic_telemetry,json_telem);
}

void publica_medicion(void){
    if (cont_int_pub>=INTERV_PUB||flag_push==1){
        telem.clear();
        flag_push=0;
        cont_int_pub=0;
        telem["Temperatura"]=random(20,30);
        send_pub();
    }
}

void publica_canales(uint8_t canal){
    telem.clear();
    if (canal==1){
        telem["Canal1"]=canal1_status;
    }
    if (canal==2){
        telem["Canal2"]=canal2_status;
    }    
    send_pub();
}