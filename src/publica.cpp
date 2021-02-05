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

//--Variables locales
uint16_t cont_int_pub=0;
StaticJsonDocument<200> pub;
char json_pub[200];
uint8_t flag_push=0;
uint8_t flag_push_att=0;


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
    if (cont_int_pub>=INTERV_PUB||flag_push==1){
        pub.clear();
        flag_push=0;
        cont_int_pub=0;
        pub["Temperatura"]=random(20,30);//sólo para testing
        send_pub(TELEMETRY);
    }
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
        //pub["Hmax"]=hmax;
        //pub["Hmin"]=hmin;
        //pub["Gain"]=gain;
        //pub["Offset"]=offset;
        //......
        send_pub(ATTRIBUTES);
    }
}