#include "publica.h"

//--Objetos externos
extern MCC_mqtt broker;

//--Variables externas
extern const char * topic_telemetry;
extern const char * topic_attributes;
extern uint8_t canal1_status;
extern uint8_t canal2_status;
extern String ssid;
extern String ubicacion;
extern String area;
extern String fversion;
extern float tmax;
extern float tmin; 
extern float offset; 
extern float gain; 
extern float temperatura;
extern uint8_t sensor;
extern uint8_t flag_publica;
extern uint8_t flag_rango;
extern uint8_t flag_pub_rango;

//--Variables locales
uint16_t cont_int_pub=0;
StaticJsonDocument<200> pub;
char json_pub[200];
uint8_t flag_push=0;
uint8_t flag_push_att=1;


void send_pub(int topico){
    serializeJson(pub, json_pub);
    switch (topico){
        case TELEMETRY:
            Serial.print("Publica Telemetria:");
            Serial.println(json_pub);
            broker.pub(topic_telemetry,json_pub);
            break;
        case ATTRIBUTES:
            Serial.print("Publica Atributos:");
            Serial.println(json_pub);
            broker.pub(topic_attributes,json_pub);
            break;
    }
}

void publica_medicion(void){
    if (flag_publica||flag_push==1){
        pub.clear();
        flag_push=0;
        flag_publica=0;
        //cont_int_pub=0;
        pub["Temperatura"]=temperatura;
        send_pub(TELEMETRY);
    }
}

void publica_rango(void){
    pub.clear();
    pub["OutRange"]=flag_rango;
    send_pub(TELEMETRY);
}

void publica_canales(uint8_t canal){
    pub.clear();
    if (canal==1){
        pub["Canal1"]=canal1_status;
    }
    if (canal==2){
        pub["Canal2"]=canal2_status;
    }    
    send_pub(TELEMETRY);
}

void publica_atributos(void){
    if (flag_push_att){
        flag_push_att=0;
        pub.clear();
        //--Atributos comunes
        pub["ssid"]=ssid;
        pub["Area"]=area;
        pub["Ubicacion"]=ubicacion;
        pub["FVersion"]=fversion;
        //--Atributos del dispositivo
        pub["Tmax"]=tmax;
        pub["Tmin"]=tmin;
        pub["Offset"]=offset;
        pub["Gain"]=gain;
        pub["Sensor"]=sensor;
        //pub["Hmax"]=hmax;
        //pub["Hmin"]=hmin;
        //......
        send_pub(ATTRIBUTES);
    }
}